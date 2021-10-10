/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#pragma once

#include <bx/uint32_t.h>
#include <bx/thread.h>
#include <bx/os.h>
#include <stdio.h>
#include <string.h>

#include "bgfx_utils.h"
#include "imgui/imgui.h"

#include "shaderc/shaderc.h"
#include "common.h"


namespace vsg{

    struct VSGDirOption{
        const char* documentDir;
        const char* appDir;
        
    };
    
    struct VSGShaderOption{
        //        compileShader[0] ="-f";
        //        compileShader[1] ="source_shader/vs_drawstress.sc";
        //        compileShader[2] ="-o";
        //        compileShader[3] ="shaders/metal/vs_drawstress.bin";
        //        compileShader[4] ="--depends";
        //        compileShader[5] ="-i";
        //        compileShader[6] ="src_shader";
        //        compileShader[7] ="--varyingdef";
        //        compileShader[8] ="source_shader/varying.def.sc";
        //        compileShader[9] ="--platform";
        //        compileShader[10] ="iOS";
        //        compileShader[11] ="--type";
        //        compileShader[12] ="fragment";
        //        compileShader[13] ="-p";
        //        compileShader[14] ="metal";
        //        compileShader[15] ="--raw";
        //        compileShader[16] ="--bin2c";
        
        
        const char* compileShaderStr[17]={"-f","source_shader/vs_drawstress.sc","-o","shaders/metal/vs_drawstress.bin","--depends","-i","src_shader","--varyingdef","source_shader/varying.def.sc","--platform","iOS","--type","fragment","-p","metal","",""};
    };

    struct VSGContext
    {
        int64_t m_timeOffset;
        std::shared_ptr<VSGDirOption> dirOption;
        std::shared_ptr<VSGShaderOption> shaderOption;
    };

    static std::shared_ptr<VSGContext> vsg_ctx = NULL;





    int init();
    void destroy();

    //编译shader到文件
    int compile_shader(const char* inputFile,const char* outFile,const char* shaderType,bool output = false,char* outputData = NULL,uint32_t* size=NULL);
    //加载shader
    bgfx::ProgramHandle load_shader_program(const char* vsFileName,const char* fsFileName);

    bool write_file(const char* _filePath, void* data, uint32_t _size,bool isDocument);
    void* load_file(const char* _filePath, uint32_t* _size,bool isDocument);
    std::string get_document_file(const char * filePath);
    std::string get_document_shader_file(const char * filePath);
    std::string get_app_file(const char * filePath);
    bool exist_document_file(const char * filePath);
    bool exist_app_file(const char * filePath);
    bool exist_file(const char * filePath);
    const char * get_cur_rt_gl_name();
    const char * get_cur_rt_shader_path();
    float get_cur_time();


}

