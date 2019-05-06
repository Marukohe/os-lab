#define MAXHANDLER 10

struct handlers{
    int event;
    handler_t handler;
}schandlers[MAXHANDLER];
