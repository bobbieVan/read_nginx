### read_nginx
-----------------
#### 开发一个模块

1. config文件编写

    只是开发http模块，需要定义三个变量:

    ngx_addon_name

    HTTP_MODULES(开发http模块)

    NGX_ADDON_SRCS

2. config文件其他变量对应的模块

    HTTP_FILTER_MODULES         --> http过滤模块

    CORE_MODULES                --> 核心模块

    EVENT_MODULES               --> 事件模块

    HTTP_MODULES                --> HTTP模块

    HTTP_HEADERS_FILER_MODULE   --> HTTP头部过滤模块
3. 利用configure脚本将定制的模块加入nginx

```
    configure --add-module=PATH
    ./configure --add-module=src/mytest/ --without-http_rewrite_module --without-http_gzip_module

```
