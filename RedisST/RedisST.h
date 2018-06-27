#pragma once

/* Redis client pool S/ingleT/on
*  @file RedisST.h
*  @brief Redis连接池单例类
*  @author S.K.
*  @date 6.11.2018 */

#include <queue>
#include <mutex>
#include <memory>
#include <functional>
#include <WinSock2.h>
#include "./include/hiredis.h"
#include "./include/win32_types_hiredis.h"

#ifdef _DEBUG
#pragma comment(lib,"./x86d/hiredis.lib")
#pragma comment(lib,"./x86d/Win32_Interop.lib")
#else
#pragma comment(lib,"./x86/hiredis.lib")
#pragma comment(lib,"./x86/Win32_Interop.lib")
#endif // _DEBUG

class RedisST
{
public:
	/* 用于管理单例、连接、连接池和命令响应资源的智能指针类型
	*  @usage
	*	RedisST::SmartST inst = RedisST::instance();
	*	RedisST::SmartConn conn(inst->get_conn(), [inst](redisContext* ctx){ inst->idle_conn(ctx); });
	*	RedisST::SmartReply reply(RedisST::exec_cmd(conn.get(), "set k v"), RedisST::free_reply); */
	using SmartST = std::shared_ptr<RedisST>;
	using SmartConn = std::unique_ptr<redisContext, std::function<void(redisContext*)>>;
	using SmartReply = std::unique_ptr<redisReply, std::function<void(redisReply*)>>;

	RedisST(const RedisST&) = delete;
	RedisST& operator=(const RedisST&) = delete;
	~RedisST() = default;

	/* 初始化唯一实例和redis地址
	*  @param redis服务器IP和端口 */
	static void init(const char* ip, int port);
	static SmartST instance();

	/* 释放所有池中连接
	*  @note 注意确认所有持有连接已放回连接池，即所有SmartConn生存周期已结束*/
	void release();

	redisContext* get_conn();
	/* 将使用完毕的redis连接放回连接池 */
	void idle_conn(redisContext* ctx);

	static redisReply* exec_cmd(redisContext* ctx, const char* cmd);
	/* @note 当reply为空时说明连接异常 */
	static bool validate_reply(SmartReply& reply);
	static void free_reply(redisReply* reply);

private:
	static SmartST st_;
	RedisST() = default;

private:
	static char ip_[16];
	static int port_;
	std::mutex mutex_;
	std::queue<redisContext*> pool_;
};