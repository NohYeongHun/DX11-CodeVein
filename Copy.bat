// ��ɾ�			�ɼ�			���� ������ �ִ� ��ġ			�纻 ������ ������ ��ġ

xcopy			/y/s		.\Engine\Public\*.*			.\EngineSDK\Inc\	

xcopy			/y			.\Engine\Bin\Engine.dll		.\Client\Bin\
xcopy			/y			.\Engine\Bin\Engine.dll		.\Tool\Bin\
xcopy			/y			.\Engine\Bin\ShaderFiles\*.* .\Client\Bin\ShaderFiles\
xcopy			/y			.\Engine\Bin\ShaderFiles\*.* .\Tool\Bin\ShaderFiles\
xcopy			/y			.\Engine\Bin\Engine.lib		.\EngineSDK\Lib\