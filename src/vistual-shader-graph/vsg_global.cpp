/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#include <sstream>
#include <sys/stat.h>
#include "vsg_global.h"

namespace vsg {

    const bgfx::Memory* load_mem(bx::FileReaderI* _reader, const char* _filePath)
    {
        if (bx::open(_reader, _filePath) )
        {
            uint32_t size = (uint32_t)bx::getSize(_reader);
            const bgfx::Memory* mem = bgfx::alloc(size+1);
            bx::read(_reader, mem->data, size);
            bx::close(_reader);
            mem->data[mem->size-1] = '\0';
            return mem;
        }

        DBG("Failed to load %s.", _filePath);
        return NULL;
    }

    const char * get_cur_rt_shader_path(){
        const char* shaderPath = "???";
        
        switch (bgfx::getRendererType() )
        {
        case bgfx::RendererType::Noop:
        case bgfx::RendererType::Direct3D9:  shaderPath = "shaders/dx9/";   break;
        case bgfx::RendererType::Direct3D11:
        case bgfx::RendererType::Direct3D12: shaderPath = "shaders/dx11/";  break;
        case bgfx::RendererType::Gnm:        shaderPath = "shaders/pssl/";  break;
        case bgfx::RendererType::Metal:      shaderPath = "shaders/metal/"; break;
        case bgfx::RendererType::Nvn:        shaderPath = "shaders/nvn/";   break;
        case bgfx::RendererType::OpenGL:     shaderPath = "shaders/glsl/";  break;
        case bgfx::RendererType::OpenGLES:   shaderPath = "shaders/essl/";  break;
        case bgfx::RendererType::Vulkan:     shaderPath = "shaders/spirv/"; break;
        case bgfx::RendererType::WebGPU:     shaderPath = "shaders/spirv/"; break;

        case bgfx::RendererType::Count:
            BX_ASSERT(false, "You should not be here!");
            break;
        }
        return shaderPath;
    }

    const char * get_cur_rt_gl_name(){
        const char* glName = "???";
        
//        {  ShadingLang::ESSL,  100,    "100_es"     },
//        {  ShadingLang::ESSL,  300,    "300_es"     },
//        {  ShadingLang::ESSL,  310,    "310_es"     },
//        {  ShadingLang::ESSL,  320,    "320_es"     },
//        {  ShadingLang::HLSL,  300,    "s_3_0"      },
//        {  ShadingLang::HLSL,  400,    "s_4_0"      },
//        {  ShadingLang::HLSL,  500,    "s_5_0"      },
//        {  ShadingLang::Metal, 1000,   "metal"      },
//        {  ShadingLang::PSSL,  1000,   "pssl"       },
//        {  ShadingLang::SpirV, 1311,   "spirv13-11" },
//        {  ShadingLang::SpirV, 1411,   "spirv14-11" },
//        {  ShadingLang::SpirV, 1512,   "spirv15-12" },
//        {  ShadingLang::SpirV, 1010,   "spirv10-10" },
//        {  ShadingLang::SpirV, 1010,   "spirv"      },
//        {  ShadingLang::GLSL,  120,    "120"        },
//        {  ShadingLang::GLSL,  130,    "130"        },
//        {  ShadingLang::GLSL,  140,    "140"        },
//        {  ShadingLang::GLSL,  150,    "150"        },
//        {  ShadingLang::GLSL,  330,    "330"        },
//        {  ShadingLang::GLSL,  400,    "400"        },
//        {  ShadingLang::GLSL,  410,    "410"        },
//        {  ShadingLang::GLSL,  420,    "420"        },
//        {  ShadingLang::GLSL,  430,    "430"        },
//        {  ShadingLang::GLSL,  440,    "440"        },
        switch (bgfx::getRendererType() )
        {
        case bgfx::RendererType::Noop:
        case bgfx::RendererType::Direct3D9:  glName = "s_3_0";   break;
        case bgfx::RendererType::Direct3D11: glName = "s_4_0";  break;
        case bgfx::RendererType::Direct3D12: glName = "s_5_0";  break;
        case bgfx::RendererType::Gnm:        glName = "pssl";  break;
        case bgfx::RendererType::Metal:      glName = "metal"; break;
        case bgfx::RendererType::Nvn:        glName = "pssl";   break;
        case bgfx::RendererType::OpenGL:     glName = "120";  break;
        case bgfx::RendererType::OpenGLES:   glName = "100_es";  break;
        case bgfx::RendererType::Vulkan:     glName = "spirv"; break;
        case bgfx::RendererType::WebGPU:     glName = "100_es"; break;

        case bgfx::RendererType::Count:
            BX_ASSERT(false, "You should not be here!");
            break;
        }
        return glName;
    }


//    bgfx::ShaderHandle load_mem_shader(char* data, uint32_t size, const char* _name)
//    {
//
//        bgfx::ShaderHandle handle = bgfx::createShader(bgfx::makeRef(data, size) );
//        if (isValid(handle) )
//        {
//            bgfx::setName(handle, _name);
//        }
//
//        return handle;
//    }

//    bgfx::ProgramHandle load_mem_program(char* vsData,const char* _vsName, uint32_t vsSize, char* fsData,  const char* _fsName, uint32_t fsSize)
//    {
//        bgfx::ShaderHandle vsh = loadMemShader(vsData, vsSize,_vsName);
//        bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
//        if (NULL != _fsName)
//        {
//            fsh = loadMemShader(fsData, fsSize,_fsName);
//        }
//
//        return bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
//    }

    bgfx::ShaderHandle load_document_shader(bx::FileReaderI* _reader, const char* _name)
    {
        auto str = get_document_shader_file(_name);
        auto filePath = str.c_str();
        
        bgfx::ShaderHandle handle = bgfx::createShader(load_mem(_reader, filePath) );
        bgfx::setName(handle, _name);

        return handle;
    }

    bgfx::ProgramHandle load_document_program(bx::FileReaderI* _reader, const char* _vsName, const char* _fsName)
    {
        bgfx::ShaderHandle vsh = load_document_shader(_reader, _vsName);
        bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
        if (NULL != _fsName)
        {
            fsh = load_document_shader(_reader, _fsName);
        }

        return bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);
    }

    bool exist_document_shader_file(const char * filePath){
        auto str = get_document_shader_file(filePath);
        auto _fileFullPath = str.c_str();
        return exist_file(_fileFullPath);
    }


    //public
    //下面这些函数是外部API

    void destroy(){
        
    }
    int init(){
        vsg_ctx =  std::make_shared<VSGContext>() ;

        vsg_ctx->m_timeOffset = bx::getHPCounter();

        vsg_ctx->dirOption =  std::make_shared<VSGDirOption>() ;
        vsg_ctx->shaderOption =  std::make_shared<VSGShaderOption>() ;


        bx::FilePath homePath = bx::FilePath(bx::Dir::Home);//(bx::Dir::Home)
        char *appDir = new char[strlen(homePath.getCPtr())];
        strcpy(appDir,homePath.getCPtr());
        homePath.join("Documents");
        char *documentDir = new char[strlen(homePath.getCPtr())];
        strcpy(documentDir,homePath.getCPtr());
        
        vsg_ctx->dirOption->appDir = appDir;
        vsg_ctx->dirOption->documentDir = documentDir;
       
        
        return 1;
    }

    std::string get_document_shader_file(const char * filePath){
        std::stringstream sstream;
        sstream<<vsg_ctx->dirOption->documentDir<<"/"<<get_cur_rt_shader_path()<<"/"<<filePath<<".bin";
        std::string ret = sstream.str();
        return ret;
    }

    std::string get_document_file(const char * filePath){
        std::stringstream sstream;
        sstream<<vsg_ctx->dirOption->documentDir<<"/"<<filePath;
        std::string ret = sstream.str();
        return ret;
    }

    std::string get_app_file(const char * filePath){
        std::stringstream sstream;
        sstream<<vsg_ctx->dirOption->appDir<<"/"<<filePath;
        return sstream.str();
    }

    

    bool exist_document_file(const char * filePath){
        auto str = get_document_file(filePath);
        auto _fileFullPath = str.c_str();
        return exist_file(_fileFullPath);
    }
    bool exist_app_file(const char * filePath){
        auto str = get_app_file(filePath);
        auto _fileFullPath = str.c_str();
        return exist_file(_fileFullPath);
    }
    bool exist_file(const char * filePath){
        struct stat my_stat;
        return (stat(filePath, &my_stat) == 0);
    }

    bool _writeFile(const char* _fileFullPath, void* data, uint32_t _size){
        bx::FilePath filePath = bx::FilePath(_fileFullPath);//(bx::Dir::Home)
        bx::StringView dir = filePath.getPath();
        if (!bx::makeAll(dir)){
            return false;
        }
        bx::FileWriterI* _writer = entry::getFileWriter();
        if (bx::open(_writer, _fileFullPath))
        {
            bx::write(_writer, data, _size);
            bx::close(_writer);
            return true;
        }
        else
        {
            DBG("Failed to open: %s.", _fileFullPath);
            return false;
        }
    }

    void* _loadFile(const char* _fileFullPath, uint32_t* _size){
        auto _reader = entry::getFileReader();
        if (bx::open(_reader, _fileFullPath) )
        {
            uint32_t size = (uint32_t)bx::getSize(_reader);
            void* data = BX_ALLOC(entry::getAllocator(), size);
            bx::read(_reader, data, size);
            bx::close(_reader);
            if (NULL != _size)
            {
                *_size = size;
            }
            return data;
        }
        else
        {
            DBG("Failed to open: %s.", _fileFullPath);
        }

        if (NULL != _size)
        {
            *_size = 0;
        }

        return NULL;
    }

    //写入文件
    bool write_file(const char* _filePath, void* data, uint32_t _size,bool document)
    {
        if(!document){
            return _writeFile(_filePath,data,_size);
        }else{
            auto str = get_document_file(_filePath);
            auto _fileFullPath = str.c_str();
            return _writeFile(_fileFullPath,data,_size);
        }
    }

    //读文件
    void* load_file(const char* _filePath,  uint32_t* _size,bool document)
    {
        if(!document){
            return _loadFile(_filePath,_size);
        }else{
            auto str = get_document_file(_filePath);
            auto _fileFullPath = str.c_str();
            if(!exist_file(_fileFullPath)){
                if (NULL != _size)
                {
                    *_size = 0;
                }
                return NULL;
            }
            return _loadFile(_fileFullPath,_size);
        }
    }

    int compile_shader(const char* inputFile,const char* outFile,const char* shaderType,bool output,char* outputData,uint32_t* size){
        const char* tmpInputFile = inputFile;
        std::string inputStr;
        if(!exist_file(inputFile)){
            if(exist_document_file(inputFile)){
                inputStr = get_document_file(inputFile);
                tmpInputFile = inputStr.c_str();
            }else{
                bx::debugOutput("input file path error");
                return 0;
            }
        }
        auto str = get_document_shader_file(outFile);//临时变量不能连续操作
        auto outFileFullPath = str.c_str();
        bx::FilePath filePath = bx::FilePath(outFileFullPath);//(bx::Dir::Home)
        bx::StringView dir = filePath.getPath();
        if (bx::makeAll(dir))
        {
            filePath = bx::FilePath(tmpInputFile);//(bx::Dir::Home)
            dir = filePath.getPath();
            filePath = bx::FilePath(dir);//(bx::Dir::Home)
            filePath.join("varying.def.sc");
            
            vsg_ctx->shaderOption->compileShaderStr[1] = tmpInputFile;
            vsg_ctx->shaderOption->compileShaderStr[3] = outFileFullPath;
            vsg_ctx->shaderOption->compileShaderStr[8] = filePath.getCPtr();
            vsg_ctx->shaderOption->compileShaderStr[12] = shaderType;
            vsg_ctx->shaderOption->compileShaderStr[14] = get_cur_rt_gl_name();
            auto success = runshaderc::compileShader(17, vsg_ctx->shaderOption->compileShaderStr,output,outputData,size);
            if(success == bx::kExitSuccess){
                bx::debugOutput("compileShader success");
            }else{
                bx::debugOutput("compileShader failed");
            }
            return success;
        }else{
            bx::debugOutput("makedir failed");
            return 0;
        }
    }


    

    
    bgfx::ProgramHandle load_shader_program(const char* vsFileName,const char* fsFileName){
        if(exist_document_shader_file(vsFileName)){
            return load_document_program(entry::getFileReader(),vsFileName, fsFileName);
        }else{
            return loadProgram(vsFileName, fsFileName);
        }
    }
    
    float get_cur_time(){
         return (float)( (bx::getHPCounter()-vsg::vsg_ctx->m_timeOffset)/double(bx::getHPFrequency() ) );
    }
}
