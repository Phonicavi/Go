## C programming -> C++ programming
语言拓展:
	这是修改了原先gcc编译的c89版本的brown
	新的brown使用g++基于c++11版本编译

## dynamic -> static linking library
静态链接:
	对于可能出现的动态链接库缺失的问题 已经对目前发现的libgcc和libstdc++两个库使用静态链接
	如果发现了新的动态链接库缺失 请及时报告
	
	注意 使用g++编译过程中的全局-static参数会对所有库使用静态链接
