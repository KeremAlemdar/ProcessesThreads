struct request {
	int id;
	char astr[64];
};
struct result {
	int id;
	int answer[1000];
	int size;
	int quit;
};

#define MQCTOS "/clienttoserver"
#define MQSTOC "/servertoclient"
