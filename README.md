# RedisST
A redis connection pool singleton using modern C++.

### Usage
    RedisST::SmartST inst = RedisST::instance();
    RedisST::SmartConn conn(inst->get_conn(), [inst](redisContext* ctx){ inst->idle_conn(ctx); });
    RedisST::SmartReply reply(RedisST::exec_cmd(conn.get(), "set k v"), RedisST::free_reply);
    
    //...
    
    RedisST::instance()->release();

### Note
The redis dll/lib/exe could be generated by [SorataKasugano/redis](https://github.com/SorataKasugano/redis) which is suitable for most versions of Windows even include xp,
it fixed some bugs bases on [MicrosoftArchive/redis](https://github.com/MicrosoftArchive/redis).
