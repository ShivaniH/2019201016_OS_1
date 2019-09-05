#define MAXY_TOKENS 500
#define MAX_TOKEN_SIZE 100

#include <string>
#include <vector>

std::vector<std::string> splitInput(int &ref, char *command);

bool checkThisInTokens(std::string special);

class backgroundProc {
    private:
        pid_t bgId;
        int exitStatus;
        int stdoutFD;
    public:
        backgroundProc();

        backgroundProc(pid_t backId);

        pid_t getBgId();

        int getExitStatus();

        void setExitStatus(int status);

        void setSTDOUT(int fd);

        int getSTDOUT();
};