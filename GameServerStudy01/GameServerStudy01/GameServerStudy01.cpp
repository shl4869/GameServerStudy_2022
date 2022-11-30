
// GameServerStudy01.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <mutex>
#include <Windows.h>

using namespace std;


// 1.6 뮤텍스 예제
const int MaxCount = 15000;
const int ThreadCount = 4;

bool IsPrimeNumber(int number)
{
    if (number == 1)
        return false;
    if (number == 2 || number == 3)
        return true;
    for (int i = 2; i < number - 1; i++)
    {
        if ((number % i) == 0)
            return false;
    }
    return true;
}

void PrintNUmbers(const vector<int>& primes)
{
    for (int v : primes)
    {
        cout << v << endl;
    }
}

// 1.7 교착상태 예제
class CriticalSection
{
    CRITICAL_SECTION m_critSec;
    public:
    CriticalSection();
    ~CriticalSection();

    void Lock();
    void Unlock();
};

class CriticalSectionLock
{
    CriticalSection* m_pCritSec;
public:
    CriticalSectionLock(CriticalSection& critSec);
    ~CriticalSectionLock();
};

CriticalSection::CriticalSection()
{
    InitializeCriticalSectionEx(&m_critSec, 0, 0);
}

CriticalSection::~CriticalSection()
{
    DeleteCriticalSection(&m_critSec);
}

void CriticalSection::Lock()
{
    EnterCriticalSection(&m_critSec);
}

void CriticalSection::Unlock()
{
    LeaveCriticalSection(&m_critSec);
}

CriticalSectionLock::CriticalSectionLock(CriticalSection& critSec)
{
    m_pCritSec = &critSec;
    m_pCritSec->Lock();
}

CriticalSectionLock::~CriticalSectionLock()
{
    m_pCritSec->Unlock();
}

int a;
CriticalSection a_mutex;
int b;
CriticalSection b_mutex;

// 메인 함수
int main()
{
    //1.6 뮤텍스 예제
       //각 스레드는 여기서 값을 꺼내 온다.
    int num = 1;
    recursive_mutex num_mutex;

    vector<int> primes;
    recursive_mutex primes_mutex;

    auto t0 = chrono::system_clock::now();

    //작동할 워커 스레드
    vector<shared_ptr<thread>> threads;

    for (int i = 0; i < ThreadCount; i++)
    {
        shared_ptr<thread> myThread(new thread([&]() 
            {
            //각 스레드의 메인 함수
            // 값을 가져올 수 있으면 루프를 돈다.
            while (true)
            {
                int n;
                {
                    lock_guard<recursive_mutex> num_lock(num_mutex);
                    n = num;
                    num++;
                }

                if (n >= MaxCount)
                    break;

                if (IsPrimeNumber(n))
                {
                    lock_guard<recursive_mutex> primes_lock(primes_mutex);
                    primes.push_back(n);
                }
            }
        }));
        //스레드 객체를 일단 갖고 있는다.
        threads.push_back(myThread);
    }

    //모든 스레드가 일을 마칠 때까지 기다린다.
    for (auto thread : threads)
    {
        thread->join();
    }
    // 끝

    auto t1 = chrono::system_clock::now();

    auto duration = chrono::duration_cast<chrono::milliseconds>(t1 - t0).count();
    cout << "Took " << duration << "miliseconds." << endl;

    //PrintNumbers(primes);

    // 1.7 교착상태 예제
    
    // th1 스레드를 시작한다.
    thread th1([]() 
    {
            int cnt1 = 0;
        while (1)
        {
            CriticalSectionLock lock(a_mutex);
            a++;
            CriticalSectionLock lock2(b_mutex);
            b++;
            cout << "t1 done.   cnt: " << cnt1++ << endl;
        }
    });

    // th2 스레드를 시작한다.
    thread th2([]() 
    {
            int cnt2 = 0;
        while(1)
        {
            CriticalSectionLock lock(b_mutex);
            b++;
            CriticalSectionLock lock2(a_mutex);
            a++;
            cout << "t2 done.   cnt: "<<cnt2++<<endl;
        }
    });

    //스레드들의 일이 끝날 때까지 기다린다.
    //사실상 무한 반복이므로 끝나지 않는다.
    th1.join();
    th2.join();

    return 0;
}


// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
