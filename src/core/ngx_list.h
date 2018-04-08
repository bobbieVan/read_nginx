
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_LIST_H_INCLUDED_
#define _NGX_LIST_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>

/**
 * 链表元素
 * 用于描述一个链表元素，一个链表元素是数组，不是单纯的一个元素
 */
typedef struct ngx_list_part_s  ngx_list_part_t;

struct ngx_list_part_s {
    void             *elts;     //指向的数组起始位置
    ngx_uint_t        nelts;    //当前已经使用的元素个数
    ngx_list_part_t  *next;     //下一个链表元素
};


typedef struct {
    ngx_list_part_t  *last;     //链表最后一个袁术
    ngx_list_part_t   part;     //链表第一个元素
    size_t            size;     //链表元素每个的大小限定
    ngx_uint_t        nalloc;   //每个链表元素(ngx_list_part_s)最对可以保存的元素个数
    ngx_pool_t       *pool;     //内存池
} ngx_list_t;


/**
 * 创建一个新链表
 * 至少会创建一个链表元素，包含n个size大小的内存块
 * @param  pool 内存池
 * @param  n
 * @param  size
 * @return
 */
ngx_list_t *ngx_list_create(ngx_pool_t *pool, ngx_uint_t n, size_t size);

/**
 * 初始化一个已有的链表
 * @param  list [description]
 * @param  pool [description]
 * @param  n    [description]
 * @param  size [description]
 * @return      [description]
 */
static ngx_inline ngx_int_t
ngx_list_init(ngx_list_t *list, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    list->part.elts = ngx_palloc(pool, n * size);
    if (list->part.elts == NULL) {
        return NGX_ERROR;//若第一个链表元素为空，失败
    }

    list->part.nelts = 0;
    list->part.next = NULL;
    list->last = &list->part;
    list->size = size;
    list->nalloc = n;
    list->pool = pool;

    return NGX_OK;
}

/**
 * 遍历链表
 * @param i
 */
/*
 *
 *  the iteration through the list:
 *
 *  part = &list.part;
 *  data = part->elts;
 *
 *  for (i = 0 ;; i++) {
 *
 *      if (i >= part->nelts) {
 *          if (part->next == NULL) {
 *              break;
 *          }
 *
 *          part = part->next;
 *          data = part->elts;
 *          i = 0;
 *      }
 *
 *      ...  data[i] ...
 *
 *  }
 */

/**
 * 添加一个元素，返回的指针是指向分配的内存，可直接赋值
 * @param  list 链表
 * @return      
 */
void *ngx_list_push(ngx_list_t *list);


#endif /* _NGX_LIST_H_INCLUDED_ */
