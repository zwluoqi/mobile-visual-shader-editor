# mobile-visual-shader-editor
一款跨平台着色器编辑工具
<a href="https://github.com/zwluoqi/mobile-visual-shader-editor/wiki/Visual-Shader">实现原理&说明</a></br>

![Visual Shader PBR](https://github.com/zwluoqi/mobile-visual-shader-editor/blob/main/images/visual-shader-graph.png)




## 依赖项&参考项
1.<a href="https://github.com/bkaradzic/bgfx">BGFX</a> 一款跨平台渲染器</br>
2.<a href="https://github.com/ocornut/imgui">IMGUI</a> 一款跨平台GUI框架</br>
3.<a href="https://github.com/jlwitthuhn/cycles-shader-editor-imgui">Cycle Graph Editor</a> 一款基于IMGUI的Node Editor</br>
4.<a href="https://github.com/assimp/assimp">assimp</a> 一款跨平台的模型加载库</br>
5.<a href="https://github.com/btzy/nativefiledialog-extended">Native Dialog File</a> 一款跨平台的文件选择库</br>
5.<a href="https://github.com/jagenjo/litegraph.js">litegraph</a> 一款web平台的shader graph</br>

## 搭建工程
### 1.下载工程
mkdir vsg</br>
cd vsg</br>
git clone https://github.com/zwluoqi/mobile-visual-shader-editor/</br>
git clone https://github.com/bkaradzic/bgfx</br>
git clone https://github.com/bkaradzic/bx</br>
git clone https://github.com/bkaradzic/assimp@5.0.0</br>
git clone https://github.com/bkaradzic/bimg</br>

### 2.生成assimp工程
cd assimp</br>
mkdir build</br>
cd build</br>
OSX: cmake .. cmake .. -G "Xcode"</br>
WIN: cmake ..</br>

### 3.安装boost库
OSX：homebrew install boost
WIN：vcpkg install boost

### 4.关联assimp
1.直接使用上面下载工程生成Assimp的动态库</br>
2.或者将下载的工程关联到mobile-visual-shader-editor工程中依赖即可</br>
*注意生成Mac APP的时候有动态库关联错误dyld: Library not loaded: @rpath/libassimp5.dylib，这是Mac自身的问题，重定向库的路径即可
```
# Type a script or drag a script file from your workspace to insert its path.
install_name_tool -id @executable_path/../Frameworks/libassimpd.5.0.0.dylib libassimpd.5.0.0.dylib
```



<a href="http://opensource.org/licenses/BSD-2-Clause" target="_blank">
<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">
</a>

	Copyright 2021-2021 ZHOU WEI
	
	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:
	
	   1. Redistributions of source code must retain the above copyright notice, this
	      list of conditions and the following disclaimer.
	
	   2. Redistributions in binary form must reproduce the above copyright notice,
	      this list of conditions and the following disclaimer in the documentation
	      and/or other materials provided with the distribution.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
	OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
	OF THE POSSIBILITY OF SUCH DAMAGE.
