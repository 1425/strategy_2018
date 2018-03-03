#include "sub.h"
#include<iostream>
#include<unistd.h>
#include<cassert>
#include<array>
#include<sys/types.h>
#include<sys/wait.h>
#include<strings.h>
#include<vector>
#include<sys/gmon.h>
#include "util.h"

using namespace std;

template<typename T>
bool operator&(vector<T> a,T b){
	for(auto elem:a){
		if(elem==b){
			return 1;
		}
	}
	return 0;
}

template<typename T>
vector<T> operator|(vector<T> a,vector<T> b){
	a|=b;
	return a;
}

array<int,2> pipe_check(){
	array<int,2> a;
	auto r=pipe(&a[0]);
	assert(r==0);
	return a;
}

void close_check(int fd){
	auto r=close(fd);
	assert(r==0);
}

void dup2_close(int fd1,int fd2){
	{
		auto r=dup2(fd1,fd2);
		assert(r>=0);
	}
	close_check(fd1);
}

using Fd=int;//file descriptor.

struct Select_result{
	vector<Fd> read,write,except;
};

ostream& operator<<(ostream& o,Select_result a){
	o<<"Select_result( ";
	#define X(A) o<<""#A<<":"<<a.A<<" ";
	X(read) X(write) X(except)
	#undef X
	return o<<")";
}

Select_result select(vector<int> read_fds,vector<int> write_fds,vector<int> except_fds){\
	assert(0);

	//tood: allow timeout
	fd_set rfds;
	FD_ZERO(&rfds);
	for(auto fd:read_fds){
		FD_SET(fd,&rfds);
	}

	fd_set wfds;
	FD_ZERO(&wfds);
	for(auto fd:write_fds){
		FD_SET(fd,&wfds);
	}

	fd_set efds;
	FD_ZERO(&efds);
	for(auto fd:except_fds){
		FD_SET(fd,&efds);
	}

	int nfds=max(read_fds|write_fds|except_fds)+1;

	struct timeval timeout;
	timeout.tv_sec=1;
	timeout.tv_usec=0;

	auto r=::select(nfds,&rfds,&wfds,&efds,&timeout);
	assert(r!=-1);//TODO: look at what the error was
	assert(r>=0);

	Select_result out;
	for(auto fd:read_fds){
		if(FD_ISSET(fd,&rfds)){
			out.read|=fd;
		}
	}
	for(auto fd:write_fds){
		if(FD_ISSET(fd,&wfds)){
			out.write|=fd;
		}
	}
	for(auto fd:except_fds){
		if(FD_ISSET(fd,&efds)){
			out.except|=fd;
		}
	}
	PRINT(out)
	PRINT(r)

	return out;
}

string read_available(Fd fd){
	stringstream ss;
	while(1){
		auto s=select({fd},{},{fd});
		assert(s.except.size()==0);
		if(s.read.empty()){
			return ss.str();
		}
		char c;
		auto r=read(fd,&c,1);
		switch(r){
			case 1:
				ss<<c;
				break;
			case 0:
				return ss.str();
			default:
				assert(0);
		}
	}
}

string read_line(Fd fd){
	stringstream ss;
	while(1){
		/*auto s=select({fd},{},{fd});
		assert(s.except.size()==0);
		if(s.read.empty()){
			return ss.str();
		}*/
		char c;
		auto r=read(fd,&c,1);
		switch(r){
			case 1:
				ss<<c;
				if(c=='\n'){
					return ss.str();
				}
				break;
			case 0:
				return ss.str();
			default:
				assert(0);
		}
	}
}


string read_str(Fd fd){
	static const auto BUF=20;
	char buf[BUF+1];
	bzero(buf,BUF+1);
	auto r=read(fd,buf,BUF);
	assert(r>=0);

	//cout<<"r:"<<r<<"\n";
	//cout<<"buf:"<<buf<<"\n";
	return string(buf);
}

struct Subprocess{
	int child_stdin,child_stdout;
	pid_t pid;

	public:
	Subprocess(){
		auto stdins=pipe_check(),stdouts=pipe_check();
		child_stdin=stdins[1];
		child_stdout=stdouts[0];

		auto f=fork();
		if(f==0){
			//child

			close_check(stdins[1]);
			close_check(stdouts[0]);

			dup2_close(stdins[0],STDIN_FILENO);
			dup2_close(stdouts[1],STDOUT_FILENO);

			execl("demo.py","demo.py",NULL);
			assert(0);
		}

		//parent
		close_check(stdins[0]);
		close_check(stdouts[1]);

		assert(f>0);
		pid=f;
	}

	void send(string s)const{
		auto r=write(child_stdin,s.c_str(),s.size());
		assert(r==int(s.size()));//todo: add loop?
	}

	string read()const{
		return read_str(child_stdout);
	}

	~Subprocess(){
		//TODO: Send some sort of signal to kill the process.

		send("exit\n");

		int status;
		waitpid(pid,&status,0);
		if(status!=0) cout<<"status:"<<status<<"\n";

		close_check(child_stdin);
		close_check(child_stdout);
	}
};

ostream& operator<<(ostream& o,Subprocess const& a){
	o<<"Subprocess( ";
	#define X(A) o<<""#A<<":"<<a.A<<" ";
	X(pid) X(child_stdin) X(child_stdout)
	#undef X
	return o<<")";
}

/*works, but extremely slow, like 500 ms
 * double skellam_cdf(double m1,double m2,double x){
	Subprocess s;
	s.send([=](){
		stringstream ss;
		ss<<m1<<" "<<m2<<" "<<x<<"\n";
		return ss.str();
	}());
	auto str=read_line(s.child_stdout);
	return atof(str.c_str());
}*/


/*not really safe, cleanup doesn't really happen.
double skellam_cdf(double m1,double m2,double x){
	static Subprocess *s=NULL;
	if(s==NULL){
		s=new Subprocess();
	}
	s->send([=](){
		stringstream ss;
		ss<<m1<<" "<<m2<<" "<<x<<"\n";
		return ss.str();
	}());
	auto str=read_line(s->child_stdout);
	return atof(str.c_str());
}*/

Skellam_cdf::Skellam_cdf():inner(new Subprocess()){
}

Skellam_cdf::~Skellam_cdf(){
	delete (Subprocess*)inner;
}

double Skellam_cdf::operator()(double m1,double m2,double x)const{
	/*static map<tuple<double,double,double>,double> cache;
	auto t=make_tuple(m1,m2,x);
	auto f=cache.find(t);
	if(f!=cache.end()){
		return f->second;
	}*/

	//auto t1=clock();
	if(m1==0) m1=.0001;
	if(m2==0) m2=.0001;

	Subprocess const& s=*(const Subprocess*)inner;
	s.send([=](){
		stringstream ss;
		ss<<x<<" "<<m1<<" "<<m2<<"\n";
		return ss.str();
	}());
	auto str=read_line(s.child_stdout);
	auto r=atof(str.c_str());
	//cache[t]=r;
	/*auto t2=clock();
	auto elapsed=(t2-t1)/CLOCKS_PER_SEC;
	PRINT(elapsed);*/
	return r;
}

#ifdef SUB_MAIN
int main(){
	Subprocess s;
	cout<<s<<"\n";
	//cout<<"read:"<<s.read();
	//s.send("lots of text here, huh?\n");
	for(auto i:range(10000)){
		PRINT(i);
		s.send("1 2 3\n");
		//cout<<"last: \""<<read_line(s.child_stdout)<<"\"";
		//cout<<"after:"<<s.read_all()<<"\n";
		//cout<<"after:"<<read_available(s.child_stdout)<<"\n";
		auto x=read_line(s.child_stdout);
		float f=atof(x.c_str());
		PRINT(f);
	}
}
#endif
