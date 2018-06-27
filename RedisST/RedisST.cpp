#include "RedisST.h"

RedisST::SmartST RedisST::st_;
char RedisST::ip_[16] = "127.0.0.1";
int RedisST::port_ = 6379;

////redisFree will crash here due to another singleton RFDMap in hiredis depot
//RedisST::~RedisST()
//{
//	while (!pool_.empty())
//	{
//		redisContext* ctx = pool_.front();
//		pool_.pop();
//		redisFree(ctx);
//	}
//}

void RedisST::init(const char * ip, int port)
{
	strcpy_s(ip_, ip);
	port_ = port;
	if (!st_) st_.reset(new RedisST());
}

RedisST::SmartST RedisST::instance()
{
	return st_;
}

void RedisST::release()
{
	while (!pool_.empty())
	{
		redisContext* ctx = pool_.front();
		pool_.pop();
		redisFree(ctx);
	}
}

redisContext * RedisST::get_conn()
{
	redisContext* ctx = nullptr;
	if (!pool_.empty())
	{// double check
		mutex_.lock();
		if (!pool_.empty())
		{
			ctx = pool_.front();
			pool_.pop();
		}
		mutex_.unlock();
	}
	if (!ctx || ctx->err)
	{
		if (ctx) redisFree(ctx);
		timeval tv{ 3,0 };
		ctx = redisConnectWithTimeout(ip_, port_, tv);
	}
	return ctx;
}

void RedisST::idle_conn(redisContext * ctx)
{
	mutex_.lock();
	pool_.push(ctx);
	mutex_.unlock();
}

redisReply * RedisST::exec_cmd(redisContext* ctx, const char * cmd)
{
	return static_cast<redisReply*>(redisCommand(ctx, cmd));
}

bool RedisST::validate_reply(SmartReply & reply)
{
	return reply && (reply->type != REDIS_REPLY_ERROR);
}

void RedisST::free_reply(redisReply * reply)
{
	freeReplyObject(reply);
}