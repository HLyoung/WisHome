

class task_impl_i
{	
public: 
	virtual ~task_impl_i(){}
	virtual void run() = 0;
	virtual task_impl_i * fork() = 0;
};

class task_impl_t:public task_impl_i
{
public:
	task_impl_t(task_func_t func_, void * arg):m_func(func_),m_arg(arg_){}
	virtual void run()
	{
		m_func(m_arg);
	}
	
	virtual task_impl_i * fork()
	{
		return new task_impl_t(m_func,m_arg);
	}
	
protected:
	task_func_t m_func;
	void* m_arg;
};


struct task_t
{
	static void dumy(void*){}
	task_t(task_func_t f_,void* d_):task_impl(new task_impl_t(f_, d_){}
	
	task_t(task_impl_i * task_impl_):task_impl(task_impl_){}
	task_t(const task_t &src):task_impl(src_.task_impl->fork()){}
	
	task_t()
	{
		task_impl = new task_impl_t(&task_t::dumy,NULL);
	}
	~task_t()
	{
		delete task_impl;
	}
	
	task_t & operator=(const task_t &src_)
	{
		delete task_impl;
		task_impl = src_.task_impl->fork();
		return *this;
	}
	
	void run()
	{
		task_impl->run();
	}
		
	task_impl_i * task_impl;
	
};

struct task_binder_t
{
	static task_t gen(void  (*func_)(void *),void *p_)
	{
		return task_t(func_,p_);
	}
	
	template<typename RET>
	static task_t gen(RET (*func_)(void)
	{
		struct lambda_t
		{
			static void task_func(void *p_)
			{
				(*(RET(*)(void))p_)();
			}
		}
		
		return task_t(lambda_t::task_func,(void*)func_);
	}
	
	template<typename FUNCT,typename ARG1>
	static task_t gen(FUNCT func_,ARG1 arg1_)
	{
		struct lambda_t:public task_impl_i
		{
			FUNCT dest_func;
			ARG1 arg1;
			lambda_t(FUNCT func_,const ARG1 &arg1_):
				dest_func(func_),
				arg1(arg1_)
			{
			}
			
			virtual void run()
			{
				(*dest_func)(arg1);
			}
			
			virtual task_impl_i *fork()
			{
				return new lambda_t(dest_func,arg1);
			}
		};
		return  task_t(new lambda_t(func_,arg1_));
		
	}
};

