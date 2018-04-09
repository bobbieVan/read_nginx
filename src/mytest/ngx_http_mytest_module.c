#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
/**
 * 整个大致流程
 * 1,nginx读取到配置文件时，发现mytest模块
 * 2,调用ngx_http_mytest_commands指定的ngx_http_mytest回调函数
 * 3,ngx_http_mytest回调时设置处理HTTP的回调函数ngx_http_mytest_handler
 *
 */
static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r);
static char *
ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
//处理配置项
static ngx_command_t ngx_http_mytest_commands[] = {
    {
        ngx_string("mytest"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF|NGX_CONF_NOARGS,
        //set回调函数，
        //char               *(*set)(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
        //当某个配置快中出现mytest时，就会回调此函数
        ngx_http_mytest,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    //空的ngx_command_t用于表示数组结束
    //#define ngx_null_command  { ngx_null_string, 0, NULL, 0, 0, NULL }
    ngx_null_command

};
//模块上下文
//解析配置文件前调用
//ngx_int_t   (*preconfiguration)(ngx_conf_t *cf);
////完成配置文件的解析后调用
//ngx_int_t   (*postconfiguration)(ngx_conf_t *cf);
////当需要存储main级别的全局配置项(直属于http{...}块的配置项)时，可以通过此回调创建存储全局配置项的结构体
//void       *(*create_main_conf)(ngx_conf_t *cf);
////初始化mian级别的配置项
//char       *(*init_main_conf)(ngx_conf_t *cf, void *conf);
////当需要存储srv级别的配置项(直属于server{...}块的配置项)时，通过此回调创建存储srv的结构体
//void       *(*create_srv_conf)(ngx_conf_t *cf);
////用于合并main和srv级别下的同名配置项
//char       *(*merge_srv_conf)(ngx_conf_t *cf, void *prev, void *conf);
////当需要存储loc级别的配置项(直属于location{...}块配置项)时，实现回调
//void       *(*create_loc_conf)(ngx_conf_t *cf);
////合并srv和loc级别下的同名配置
//char       *(*merge_loc_conf)(ngx_conf_t *cf, void *prev, void *conf);
static ngx_http_module_t ngx_http_mytest_module_ctx = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
//新模块定义
ngx_module_t ngx_http_mytest_module = {
    NGX_MODULE_V1,
    //ctx,对于HTTP模块来说，ctx必须是ngx_http_module_t接口
    &ngx_http_mytest_module_ctx,
    //commands,
    ngx_http_mytest_commands,
    //定义http模块时，必须设置成NGX_HTTP_MODULE
    NGX_HTTP_MODULE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
	NULL,
    NGX_MODULE_V1_PADDING
};

//配置项对应的回调函数
static char *
ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);

    //在NGX_HTTP_CONTENT_PHASE阶段会调用此回调函数
    clcf->handler = ngx_http_mytest_handler;

    return NGX_CONF_OK;
}

//实际完成处理的回调函数
/*
 * r 是nginx已经处理完了的http请求头
 */
static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r)
{
    if (!(r->method & (NGX_HTTP_GET | NGX_HTTP_HEAD))) {
        //非法请求方式 状态码 405
        return NGX_HTTP_NOT_ALLOWED;
    }
    ngx_int_t rc = ngx_http_discard_request_body(r);
    if (rc != NGX_OK) {
        return rc;
    }

    ngx_str_t type = ngx_string("text/plain");
    ngx_str_t response = ngx_string("Hello World");
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = response.len;
    r->headers_out.content_type = type;
    
    rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }

    ngx_buf_t *b;
    b = ngx_create_temp_buf(r->pool, response.len);
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    ngx_memcpy(b->pos, response.data, response.len);
    b->last = b->pos + response.len;
    b->last_buf = 1;

    ngx_chain_t out;
    out.buf = b;
    out.next = NULL;

    return ngx_http_output_filter(r, &out);
}
