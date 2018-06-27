#pragma once

/* Redis client pool S/ingleT/on
*  @file RedisST.h
*  @brief Redis���ӳص�����
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
	/* ���ڹ����������ӡ����ӳغ�������Ӧ��Դ������ָ������
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

	/* ��ʼ��Ψһʵ����redis��ַ
	*  @param redis������IP�Ͷ˿� */
	static void init(const char* ip, int port);
	static SmartST instance();

	/* �ͷ����г�������
	*  @note ע��ȷ�����г��������ѷŻ����ӳأ�������SmartConn���������ѽ���*/
	void release();

	redisContext* get_conn();
	/* ��ʹ����ϵ�redis���ӷŻ����ӳ� */
	void idle_conn(redisContext* ctx);

	static redisReply* exec_cmd(redisContext* ctx, const char* cmd);
	/* @note ��replyΪ��ʱ˵�������쳣 */
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