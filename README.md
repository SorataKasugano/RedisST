# RedisST
A redis connection pool singleton using modern C++.

###Usage
  RedisST::SmartST inst = RedisST::instance();
  RedisST::SmartConn conn(inst->get_conn(), [inst](redisContext* ctx){ inst->idle_conn(ctx); });
  RedisST::SmartReply reply(RedisST::exec_cmd(conn.get(), "set k v"), RedisST::free_reply);
  //...
  RedisST::instance()->release();

###Note
This project using [SorataKasugano/redis](https://github.com/SorataKasugano/redis) to generate dll/lib/exe 
which is suitable for most versions of Windows even include xp,
it fixed some bugs base on [MicrosoftArchive/redis](https://github.com/MicrosoftArchive/redis).
