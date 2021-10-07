/*
  Native File Dialog Extended
  Repository: https://github.com/btzy/nativefiledialog-extended
  License: Zlib
  Authors: Bernard Teo, Michael Labbe
 */

#include "nfd.h"

#if BX_PLATFORM_IOS
#include <assert.h>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/CAEAGLLayer.h>
#import <MobileCoreServices/MobileCoreServices.h>



@interface UNativeFilePicker:NSObject
+ (void)pickFiles:(BOOL)allowMultipleSelection withUTIs:(NSArray<NSString *> *)allowedUTIs;
+ (void)exportFiles:(NSArray<NSURL *> *)paths;
+ (int)canPickMultipleFiles;
+ (int)isFilePickerBusy;
+ (char *)convertExtensionToUTI:(NSString *)extension;
@end

@implementation UNativeFilePicker

static UIDocumentPickerViewController *filePicker;
static BOOL pickingMultipleFiles;
static int filePickerState = 0; // 0 -> none, 1 -> showing, 2 -> finished
static nfdresult_t filePickerResult;
static int filePickerMode = 0;// 0->none  1->sigle 2->multiple 3->save 4->cancle
static char filePickerPath[1024];


static NSMutableArray<NSString *> * BuildAllowedFileTypes(const nfdnfilteritem_t* filterList,
                                      nfdfiltersize_t filterCount) {
    // Commas and semicolons are the same thing on this platform

    NSMutableArray<NSString *> *buildFilterList = [NSMutableArray arrayWithCapacity:1];
//    NSMutableArray* buildFilterList = [[NSMutableArray alloc] init];

    for (nfdfiltersize_t filterIndex = 0; filterIndex != filterCount; ++filterIndex) {
        // this is the spec to parse (we don't use the friendly name on OS X)
        const nfdnchar_t* filterSpec = filterList[filterIndex].spec;

        const nfdnchar_t* p_currentFilterBegin = filterSpec;
        for (const nfdnchar_t* p_filterSpec = filterSpec; *p_filterSpec; ++p_filterSpec) {
            if (*p_filterSpec == ',') {
                // add the extension to the array
                NSString* filterStr = [[[NSString alloc]
                    initWithBytes:(const void*)p_currentFilterBegin
                           length:(sizeof(nfdnchar_t) * (p_filterSpec - p_currentFilterBegin))
                         encoding:NSUTF8StringEncoding] autorelease];
                [buildFilterList addObject:filterStr];
                p_currentFilterBegin = p_filterSpec + 1;
            }
        }
        // add the extension to the array
        NSString* filterStr = [NSString stringWithUTF8String:p_currentFilterBegin];
        [buildFilterList addObject:filterStr];
    }

//    NSArray* returnArray = [NSArray arrayWithArray:buildFilterList];
//
//    [buildFilterList release];

    assert([buildFilterList count] != 0);

    return buildFilterList;
}

static NSMutableArray<NSURL *> * BuildURLAllowedFileTypes(const nfdnfilteritem_t* filterList,
                                      nfdfiltersize_t filterCount) {
    // Commas and semicolons are the same thing on this platform

    NSMutableArray<NSURL *> *buildFilterList = [NSMutableArray arrayWithCapacity:1];
//    NSMutableArray* buildFilterList = [[NSMutableArray alloc] init];

    for (nfdfiltersize_t filterIndex = 0; filterIndex != filterCount; ++filterIndex) {
        // this is the spec to parse (we don't use the friendly name on OS X)
        const nfdnchar_t* filterSpec = filterList[filterIndex].spec;

        const nfdnchar_t* p_currentFilterBegin = filterSpec;
        for (const nfdnchar_t* p_filterSpec = filterSpec; *p_filterSpec; ++p_filterSpec) {
            if (*p_filterSpec == ',') {
                // add the extension to the array
                NSString* filterStr = [[[NSString alloc]
                    initWithBytes:(const void*)p_currentFilterBegin
                           length:(sizeof(nfdnchar_t) * (p_filterSpec - p_currentFilterBegin))
                         encoding:NSUTF8StringEncoding] autorelease];
                [buildFilterList addObject:[NSURL fileURLWithPath:filterStr]];
                p_currentFilterBegin = p_filterSpec + 1;
            }
        }
        // add the extension to the array
        NSString* filterStr = [NSString stringWithUTF8String:p_currentFilterBegin];
        [buildFilterList addObject:[NSURL fileURLWithPath:filterStr]];
    }

//    NSArray* returnArray = [NSArray arrayWithArray:buildFilterList];
//
//    [buildFilterList release];

    assert([buildFilterList count] != 0);

    return buildFilterList;
}

static UIViewController* getTopController(UIWindow* app){
    UIViewController* topController = [app rootViewController];
    while (topController.presentedViewController != nil) {
        topController = topController.presentedViewController;
    }
    return topController;
}

static UIWindow* getCurrentWindow()
{
    return[UIApplication sharedApplication].keyWindow;
    //下面代码导致第二次打开有问题
//
////    if([[[UIApplication sharedApplication] delegate] window]) {
////        return[[[UIApplication sharedApplication] delegate] window];
////    }else{
//
//        if(@available(iOS 13.0, *)) {
//
//            NSArray *array =[[[UIApplication sharedApplication] connectedScenes] allObjects];
//
//            UIWindowScene* windowScene = (UIWindowScene*)array[0];
//
//            //如果是普通App开发，可以使用
//
////            SceneDelegate * delegate = (SceneDelegate *)windowScene.delegate;
//
////            UIWindow * mainWindow = delegate.window;
//
//            //由于在sdk开发中，引入不了SceneDelegate的头文件，所以需要用kvc获取宿主app的window.
//
//            UIWindow* mainWindow = [windowScene valueForKeyPath:@"delegate.window"];
//
//            if(mainWindow) {
//
//                return mainWindow;
//
//            }else{
//
//                return[UIApplication sharedApplication].windows.lastObject;
//
//            }
//        }else{
//
//            // Fallback on earlier versions
//            return[UIApplication sharedApplication].keyWindow;
//        }
////    }

}


static const char* g_errorstr = NULL;

static void NFDi_SetError(const char* msg) {
    g_errorstr = msg;
}

static void* NFDi_Malloc(size_t bytes) {
    void* ptr = malloc(bytes);
    if (!ptr) NFDi_SetError("NFDi_Malloc failed.");

    return ptr;
}

static void NFDi_Free(void* ptr) {
    assert(ptr);
    free(ptr);
}


static nfdresult_t CopyUtf8String(const char* utf8Str, nfdnchar_t** out) {
    // byte count, not char count
    size_t len = strlen(utf8Str);

    // Too bad we have to use additional memory for all the result paths,
    // because we cannot reconstitute an NSString from a char* to release it properly.
    *out = (nfdnchar_t*)NFDi_Malloc(len + 1);
    if (*out) {
        strcpy(*out, utf8Str);
        return NFD_OKAY;
    }

    return NFD_ERROR;
}

/* public */

const char* NFD_GetError(void) {
    return g_errorstr;
}

void NFD_FreePathN(nfdnchar_t* filePath) {
    NFDi_Free((void*)filePath);
}


nfdresult_t NFD_Init(void) {
    nfdresult_t result = NFD_OKAY;
    return result;
}

/* call this to de-initialize NFD, if NFD_Init returned NFD_OKAY */
void NFD_Quit(void) {
    
}

nfdresult_t NFD_OpenDialogN(nfdnchar_t** outPath,
                            const nfdnfilteritem_t* filterList,
                            nfdfiltersize_t filterCount,
                            const nfdnchar_t* defaultPath) {
    
    nfdresult_t result = NFD_CANCEL;
    
    NSMutableArray<NSString *> *allowedUTIs = BuildAllowedFileTypes(filterList, filterCount);

    [UNativeFilePicker pickFiles:NO withUTIs:allowedUTIs];
    
    return result;
}

nfdresult_t NFD_OpenDialogMultipleN(const nfdpathset_t** outPaths,
                                    const nfdnfilteritem_t* filterList,
                                    nfdfiltersize_t filterCount,
                                    const nfdnchar_t* defaultPath) {
    nfdresult_t result = NFD_CANCEL;
    NSMutableArray<NSString *> *allowedUTIs = BuildAllowedFileTypes(filterList, filterCount);
       
    [UNativeFilePicker pickFiles:YES withUTIs:allowedUTIs];
    
    return result;
}

nfdresult_t NFD_SaveDialogN(nfdnchar_t** outPath,
                            const nfdnfilteritem_t* filterList,
                            nfdfiltersize_t filterCount,
                            const nfdnchar_t* defaultPath,
                            const nfdnchar_t* defaultName) {
    nfdresult_t result = NFD_CANCEL;
    
    NSMutableArray<NSURL *> *allowedUTIs = BuildURLAllowedFileTypes(filterList, filterCount);
    
    [UNativeFilePicker exportFiles:allowedUTIs];
    
    return result;
}

nfdresult_t NFD_PickFolderN(nfdnchar_t** outPath, const nfdnchar_t* defaultPath) {
    nfdresult_t result = NFD_CANCEL;
    
    return result;
}

nfdresult_t NFD_PathSet_GetCount(const nfdpathset_t* pathSet, nfdpathsetsize_t* count) {
    const NSArray* urls = (const NSArray*)pathSet;
    *count = [urls count];
    return NFD_OKAY;
}

nfdresult_t NFD_PathSet_GetPathN(const nfdpathset_t* pathSet,
                                 nfdpathsetsize_t index,
                                 nfdnchar_t** outPath) {
    const NSArray* urls = (const NSArray*)pathSet;

    @autoreleasepool {
        // autoreleasepool needed because UTF8String method might use the pool
        const NSURL* url = [urls objectAtIndex:index];
        const char* utf8Path = [[url path] UTF8String];
        return CopyUtf8String(utf8Path, outPath);
    }
}

void NFD_PathSet_Free(const nfdpathset_t* pathSet) {
    const NSArray* urls = (const NSArray*)pathSet;
    [urls release];
}

nfdresult_t NFD_PathSet_GetEnum(const nfdpathset_t* pathSet, nfdpathsetenum_t* outEnumerator) {
    const NSArray* urls = (const NSArray*)pathSet;

    @autoreleasepool {
        // autoreleasepool needed because NSEnumerator uses it
        NSEnumerator* enumerator = [urls objectEnumerator];
        [enumerator retain];
        outEnumerator->ptr = (void*)enumerator;
    }

    return NFD_OKAY;
}

void NFD_PathSet_FreeEnum(nfdpathsetenum_t* enumerator) {
    NSEnumerator* real_enum = (NSEnumerator*)enumerator->ptr;
    [real_enum release];
}

nfdresult_t NFD_PathSet_EnumNextN(nfdpathsetenum_t* enumerator, nfdnchar_t** outPath) {
    NSEnumerator* real_enum = (NSEnumerator*)enumerator->ptr;

    @autoreleasepool {
        // autoreleasepool needed because NSURL uses it
        const NSURL* url = [real_enum nextObject];
        if (url) {
            const char* utf8Path = [[url path] UTF8String];
            return CopyUtf8String(utf8Path, outPath);
        } else {
            *outPath = NULL;
            return NFD_OKAY;
        }
    }
}

+ (void)pickFiles:(BOOL)allowMultipleSelection withUTIs:(NSArray<NSString *> *)allowedUTIs
{
    filePicker = [[UIDocumentPickerViewController alloc] initWithDocumentTypes:allowedUTIs inMode:UIDocumentPickerModeOpen];
    filePicker.delegate = (id) self;
    
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 110000
    if( [[[UIDevice currentDevice] systemVersion] compare:@"11.0" options:NSNumericSearch] != NSOrderedAscending )
        if (@available(iOS 11.0, *)) {
            filePicker.allowsMultipleSelection = allowMultipleSelection;
        } else {
            // Fallback on earlier versions
        }
#endif
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 130000
    // Show file extensions if possible
    if( [[[UIDevice currentDevice] systemVersion] compare:@"13.0" options:NSNumericSearch] != NSOrderedAscending )
        if (@available(iOS 13.0, *)) {
            filePicker.shouldShowFileExtensions = YES;
        } else {
            // Fallback on earlier versions
        }
#endif
    
    pickingMultipleFiles = allowMultipleSelection;
    filePickerState = 1;
    UIWindow* app = getCurrentWindow();
    UIViewController* viewController = getTopController(app);
    
    //[filePicker dismissViewControllerAnimated:NO completion:nil];

    [viewController presentViewController:filePicker animated:NO completion:^{ filePickerState = 0; }];
}

+ (void)exportFiles:(NSArray<NSURL *> *)paths
{
    if( paths != nil && [paths count] > 0 )
    {
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 110000
        if ([paths count] > 1 && [[[UIDevice currentDevice] systemVersion] compare:@"11.0" options:NSNumericSearch] != NSOrderedAscending)
            if (@available(iOS 11.0, *)) {
                filePicker = [[UIDocumentPickerViewController alloc] initWithURLs:paths inMode:UIDocumentPickerModeExportToService];
            } else {
                // Fallback on earlier versions
            }
        else
#endif
            filePicker = [[UIDocumentPickerViewController alloc] initWithURL:paths[0] inMode:UIDocumentPickerModeExportToService];
        
        filePicker.delegate = (id) self;
        
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 130000
        // Show file extensions if possible
        if( [[[UIDevice currentDevice] systemVersion] compare:@"13.0" options:NSNumericSearch] != NSOrderedAscending )
            if (@available(iOS 13.0, *)) {
                filePicker.shouldShowFileExtensions = YES;
            } else {
                // Fallback on earlier versions
            }
#endif
        
        filePickerState = 1;
        UIWindow* app = getCurrentWindow();
        UIViewController* viewController = getTopController(app);
        
        [viewController presentViewController:filePicker animated:NO completion:^{ filePickerState = 0; }];
    }
}

+ (int)canPickMultipleFiles
{
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 110000
    if( [[[UIDevice currentDevice] systemVersion] compare:@"11.0" options:NSNumericSearch] != NSOrderedAscending )
        return 1;
#endif
    
    return 0;
}

+ (int)isFilePickerBusy
{
    if( filePickerState == 2 )
        return 1;
    
    if( filePicker != nil )
    {
        UIWindow* app = getCurrentWindow();
        UIViewController* viewController = getTopController(app);
        if( filePickerState == 1 || [filePicker presentingViewController] == viewController )
            return 1;
        else
        {
            filePicker = nil;
            return 0;
        }
    }
    else
        return 0;
}

// Credit: https://lists.apple.com/archives/cocoa-dev/2012/Jan/msg00052.html
+ (char *)convertExtensionToUTI:(NSString *)extension
{
    CFStringRef fileUTI = UTTypeCreatePreferredIdentifierForTag( kUTTagClassFilenameExtension, (__bridge CFStringRef) extension, NULL );
    char *result = [self getCString:(__bridge NSString *) fileUTI];
    CFRelease( fileUTI );
    
    return result;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
+ (void)documentPicker:(UIDocumentPickerViewController *)controller didPickDocumentAtURL:(NSURL *)url
{
    [self documentPickerCompleted:controller documents:@[url]];
}
#pragma clang diagnostic pop

+ (void)documentPicker:(UIDocumentPickerViewController *)controller didPickDocumentsAtURLs:(NSArray<NSURL *> *)urls
{
    [self documentPickerCompleted:controller documents:urls];
}

+ (void)documentPickerCompleted:(UIDocumentPickerViewController *)controller documents:(NSArray<NSURL *> *)urls
{
    filePicker = nil;
    filePickerState = 2;
    
    if( controller.documentPickerMode == UIDocumentPickerModeOpen )
    {
        if( !pickingMultipleFiles || [urls count] <= 1 )
        {
            const char* filePath;
            if( [urls count] == 0 )
                filePath = "";
            else
                filePath = [self getCString:[urls[0] path]];
            
            if( pickingMultipleFiles )
            {
                strcpy(filePickerPath, filePath);
                filePickerResult = NFD_OKAY;
                filePickerMode = 2;
            }
//                UnitySendMessage( "FPResultCallbackiOS", "OnMultipleFilesPicked", filePath );
            else
            {
                strcpy(filePickerPath, filePath);
                filePickerResult = NFD_OKAY;
                filePickerMode = 1;
            }
//                UnitySendMessage( "FPResultCallbackiOS", "OnFilePicked", filePath );
        }
        else
        {
            NSMutableArray<NSString *> *filePaths = [NSMutableArray arrayWithCapacity:[urls count]];
            for( int i = 0; i < [urls count]; i++ )
                [filePaths addObject:[urls[i] path]];
            filePickerMode = 2;
            filePickerResult = NFD_ERROR;
            assert("not support!");
            
//            UnitySendMessage( "FPResultCallbackiOS", "OnMultipleFilesPicked", [self getCString:[filePaths componentsJoinedByString:@">"]] );
        }
    }
    else if( controller.documentPickerMode == UIDocumentPickerModeExportToService )
    {
        if( [urls count] > 0 ){
            filePickerMode = 3;
            filePickerResult = NFD_OKAY;
        }
//            UnitySendMessage( "FPResultCallbackiOS", "OnFilesExported", "1" );
        else
        {
            filePickerMode = 3;
            filePickerResult = NFD_CANCEL;
        }
//            UnitySendMessage( "FPResultCallbackiOS", "OnFilesExported", "0" );
    }
    
    [controller dismissViewControllerAnimated:NO completion:nil];
}

+ (void)documentPickerWasCancelled:(UIDocumentPickerViewController *)controller
{
    filePicker = nil;
    filePickerMode = 4;
    filePickerResult = NFD_CANCEL;
//    UnitySendMessage( "FPResultCallbackiOS", "OnOperationCancelled", "" );
    
    [controller dismissViewControllerAnimated:NO completion:nil];
}

// Credit: https://stackoverflow.com/a/37052118/2373034
+ (char *)getCString:(NSString *)source
{
    if( source == nil )
        source = @"";
    
    const char *sourceUTF8 = [source UTF8String];
    char *result = (char*) malloc( strlen( sourceUTF8 ) + 1 );
    strcpy(result, sourceUTF8);
    
    return result;
}

@end

extern "C" void _NativeFilePicker_PickFile( const char* UTIs[], int UTIsCount )
{
    NSMutableArray<NSString *> *allowedUTIs = [NSMutableArray arrayWithCapacity:UTIsCount];
    for( int i = 0; i < UTIsCount; i++ )
        [allowedUTIs addObject:[NSString stringWithUTF8String:UTIs[i]]];
    
    [UNativeFilePicker pickFiles:NO withUTIs:allowedUTIs];
}

extern "C" void _NativeFilePicker_PickMultipleFiles( const char* UTIs[], int UTIsCount )
{
    NSMutableArray<NSString *> *allowedUTIs = [NSMutableArray arrayWithCapacity:UTIsCount];
    for( int i = 0; i < UTIsCount; i++ )
        [allowedUTIs addObject:[NSString stringWithUTF8String:UTIs[i]]];
    
    [UNativeFilePicker pickFiles:YES withUTIs:allowedUTIs];
}

extern "C" void _NativeFilePicker_ExportFiles( const char* files[], int filesCount )
{
    NSMutableArray<NSURL *> *paths = [NSMutableArray arrayWithCapacity:filesCount];
    for( int i = 0; i < filesCount; i++ )
    {
        NSString *filePath = [NSString stringWithUTF8String:files[i]];
        [paths addObject:[NSURL fileURLWithPath:filePath]];
    }
    
    [UNativeFilePicker exportFiles:paths];
}

extern "C" int _NativeFilePicker_CanPickMultipleFiles()
{
    return [UNativeFilePicker canPickMultipleFiles];
}

extern "C" int _NativeFilePicker_IsFilePickerBusy()
{
    return [UNativeFilePicker isFilePickerBusy];
}

extern "C" char* _NativeFilePicker_ConvertExtensionToUTI( const char* extension )
{
    return [UNativeFilePicker convertExtensionToUTI:[NSString stringWithUTF8String:extension]];
}

#endif
