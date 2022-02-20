#pragma once
class LocalContext {
public:
    int getThreadRpCount() { return threadRpcCount; }
private:
    int threadRpcCount;
};