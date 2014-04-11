ULib_Canvas
============

A render plugin used for Unity3D, the sample is a eddy particle effects.

一个允许开发者在Unity3D工程中直接插入OpenGL, Direct3D 代码的插件，可靠的管理底层渲染资源，并自动将每份显存数据独立交换到Unity渲染场景的每张纹理上。
允许开启附属渲染窗体。也接受Unity下传渲染数据作为VideoWindow使用。

演示代码是一个漩涡状的粒子效果，重力从鼠标（手指触点处）塌陷。

依赖项目：Gut, RenderingFramework.DirectX