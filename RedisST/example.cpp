#include "RedisST.h"

void redis_test()
{
	RedisST::SmartST inst = RedisST::instance();
	RedisST::SmartConn conn(inst->get_conn(), [inst](redisContext* ctx) { inst->idle_conn(ctx); });
	if (!conn || conn->err)
	{
		printf("Fetch redis connection failed: %s", conn ? conn->errstr : "unknown error");
	}
	RedisST::SmartReply reply(nullptr, RedisST::free_reply);

	char cmd[10] = "exists k1", value[10];
	reply.reset(RedisST::exec_cmd(conn.get(), cmd));
	if (!RedisST::validate_reply(reply)) goto _error_;

	if (!reply->integer)
	{
		strcpy_s(cmd, "set k1 v1");
		reply.reset(RedisST::exec_cmd(conn.get(), cmd));
		if (!RedisST::validate_reply(reply)) goto _error_;
	}

	strcpy_s(cmd, "get k1");
	reply.reset(RedisST::exec_cmd(conn.get(), cmd));
	if (!RedisST::validate_reply(reply)) goto _error_;
	if (reply->str) strcpy_s(value, reply->str);
	printf("value: %s", value);

	strcpy_s(cmd, "del k1");
	reply.reset(RedisST::exec_cmd(conn.get(), cmd));
	if (!RedisST::validate_reply(reply)) goto _error_;
	return;

_error_:
	printf("Perform redis cmd failed[%s]: %s", reply ? reply->str : "connect interrupted", cmd);
}

int main()
{
	RedisST::init("127.0.0.1", 6379);
	redis_test();
	RedisST::instance()->release();
	return 0;
}