/**
 * 使用nginx自带的配置解析函数解析配置信息
 */
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_string.h>
#include <ngx_http_request.h>
#include <ngx_hash.h>


ngx_str_t* tmp;
//定义配置结构体
typedef struct {
    ngx_str_t       my_str;
    ngx_int_t       my_num;
    ngx_flag_t      my_flag;
//    size_t          my_size;
//    ngx_array_t*    my_str_array;
//    ngx_array_t*    my_keyval;
//    off_t           my_off;
//    ngx_msec_int_t  my_msec;
//    time_t          my_sec;
//    ngx_bufs_t      my_bufs;
//    ngx_uint_t      my_enum_seq;
//    ngx_uid_t       my_bitmask;
//    ngx_uint_t      my_access;
//    ngx_path_t*     my_path;
}ngx_http_mytest_conf_t;

static ngx_int_t ngx_http_autoconfig_handler(ngx_http_request_t *r)
{
    if (!(r->method & (NGX_HTTP_GET | NGX_HTTP_HEAD))) {
        //非法请求方式 状态码 405
        return NGX_HTTP_NOT_ALLOWED;
    }
    //丢弃客户端发送来的HTTP包体内容
    ngx_int_t rc = ngx_http_discard_request_body(r);
    if (rc != NGX_OK) {
        return rc;
    }
    
    ngx_str_t type = ngx_string("text/plain");
    ngx_http_mytest_conf_t* mcf = r->loc_conf[0];
//    ngx_str_t response = ngx_string(r->method_name.data);
    ngx_str_t response = *tmp;
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = response.len;
    r->headers_out.content_type = type;
    //自定义响应头
    ngx_table_elt_t* p = ngx_list_push(&r->headers_out.headers);
    p->hash = 1;
    p->key.len = sizeof("codelover")-1;
    p->key.data = (u_char*)"codelover";
    p->value.len = sizeof("codelover")-1;
    p->value.data = (u_char*)"codelover";
    //r->connection->log
    //发送响应头

    ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0, "%V",mcf->my_str);
    rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }

    ngx_buf_t *b;
    //r->pool内存池
    b = ngx_create_temp_buf(r->pool, response.len);
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    ngx_memcpy(b->pos, response.data, response.len);
    //必须设置好last指针，如果last和pos相等，是不会发送的
    b->last = b->pos + response.len;
    //声明这是最后一块缓冲区
    b->last_buf = 1;

    ngx_chain_t out;
    out.buf = b;
    out.next = NULL;

    return ngx_http_output_filter(r, &out);
}


//配置项对应的回调函数
static char *
ngx_http_autoconfig(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
    ngx_http_core_loc_conf_t *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);

    //在NGX_HTTP_CONTENT_PHASE阶段会调用此回调函数
    clcf->handler = ngx_http_autoconfig_handler;

    return NGX_CONF_OK;
}
static char *
ngx_conf_set_str_slot1(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {


    char* r = ngx_conf_set_str_slot(cf,cmd,conf);
    ngx_http_mytest_conf_t* mf = conf;
    tmp = &mf->my_str;
    return r;
}

static ngx_command_t ngx_http_autoconfig_commands[] = {
        //根据teat_falg 配置设定 my_flag的值
        {
                //表明此处根据配置文件的test_flag来解析
                ngx_string("test_flag"),
                //NGX_HTTP_LOC_CONF限制出现在location配置中
                //NGX_CONF_FLAG限制只能配置on or off
                NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
                //使用nginx自带的解析函数解析my_flag，类型ngx_flag_t
                ngx_conf_set_flag_slot,
                NGX_HTTP_LOC_CONF_OFFSET,
                offsetof(ngx_http_mytest_conf_t, my_flag),
                NULL
        },

        //根据teat_str 配置设定 my_str的值
        {
                //表明此处根据配置文件的test_flag来解析
                ngx_string("test_str"),
                //NGX_HTTP_LOC_CONF限制出现在location配置中
                //NGX_CONF_TAKE1限制必须携带一个参数
                NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
                //使用nginx自带的解析函数解析my_flag，类型ngx_flag_t
                ngx_conf_set_str_slot1,
                NGX_HTTP_LOC_CONF_OFFSET,
                offsetof(ngx_http_mytest_conf_t, my_str),
                NULL
        },

        //根据teat_num 配置设定 my_num的值
        {
                //表明此处根据配置文件的test_flag来解析
                ngx_string("test_num"),
                //NGX_HTTP_LOC_CONF限制出现在location配置中
                //NGX_CONF_TAKE1限制必须携带一个参数
                NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
                //使用nginx自带的解析函数解析my_flag，类型ngx_flag_t
                ngx_conf_set_num_slot,
                NGX_HTTP_LOC_CONF_OFFSET,
                offsetof(ngx_http_mytest_conf_t, my_num),
                NULL
        },

        {
                ngx_string("autoconfig"),
                NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF|NGX_CONF_NOARGS,
                //set回调函数，
                //char               *(*set)(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
                //当某个配置快中出现mytest时，就会回调此函数
                ngx_http_autoconfig,
                NGX_HTTP_LOC_CONF_OFFSET,
                0,
                NULL
        },
        //空的ngx_command_t用于表示数组结束
        //#define ngx_null_command  { ngx_null_string, 0, NULL, 0, 0, NULL }
        ngx_null_command

};





static void* ngx_http_autoconfig_create_loc_conf(ngx_conf_t* cf) {
    ngx_http_mytest_conf_t *mycf;
    mycf = (ngx_http_mytest_conf_t*)ngx_pcalloc(cf->pool, sizeof(ngx_http_mytest_conf_t));
    if(mycf == NULL) {
        return NULL;
    }

    //必须初始化，否则会有些地方解析会出错
    mycf->my_flag = NGX_CONF_UNSET;
    mycf->my_num = NGX_CONF_UNSET;
//    mycf->my_str_array = NGX_CONF_UNSET_PTR;
//    mycf->my_keyval = NULL;
//    mycf->my_off = NGX_CONF_UNSET;
//    mycf->my_msec = NGX_CONF_UNSET_MSEC;
//    mycf->my_sec = NGX_CONF_UNSET;
//    mycf->my_size = NGX_CONF_UNSET_SIZE;
    return mycf;

}
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
static ngx_http_module_t ngx_http_autoconfig_module_ctx = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        //此回调函数产生的结构体会存在ngx_http_conf_ctx_t 中的loc_conf中
        //typedef struct {
        //    //指针数组，每个元素指向所有HTTP模块create_main_conf方法产生的结构体
        //    void        **main_conf;
        //    //..............................create_srv_conf..............
        //    void        **srv_conf;
        //    //..............................create_loc_conf..............
        //    void        **loc_conf;
        //} ngx_http_conf_ctx_t;
        ngx_http_autoconfig_create_loc_conf,
        NULL
};
//新模块定义
ngx_module_t ngx_http_autoconfig_module = {
        NGX_MODULE_V1,
        //ctx,对于HTTP模块来说，ctx必须是ngx_http_module_t接口
        &ngx_http_autoconfig_module_ctx,
        //commands,
        ngx_http_autoconfig_commands,
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