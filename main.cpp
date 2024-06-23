#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <cmath>

std::string storageFileName = "storage.txt";

bool debug = false;
bool paramsSuccess = false;

sem_t producerSemaphore, consumerSemaphore, criticalSectionSemaphore;
int storage, products;

int k, n, m, a, b, c, d;

int getRandomInt(int a, int b)
{
    return rand() % (b - a + 1) + a;
}

void *producer(void *arg)
{
    long thread_id = (long)arg;

    while (1)
    {
        sem_wait(&producerSemaphore);
        sem_wait(&criticalSectionSemaphore);

        if (debug)
            std::cout << "Producer " << thread_id << " entered the critical section\n";
        std::ifstream storageFileIn(storageFileName);
        storageFileIn >> storage;
        storageFileIn.close();
        products = getRandomInt(c, d);
        std::ofstream storageFileOut(storageFileName);
        std::fstream logFile("log.txt", std::ios::app);
        std::string fileName = "producer" + std::to_string(thread_id) + "log.txt";
        std::fstream localLogFile(fileName, std::ios::app);
        if (storage + products <= k)
        {
            storage += products;
            logFile << "Producer " << thread_id << " delivered " << products << " piece(s), storage: " << storage << " piece(s)\n";
            localLogFile << "I delivered " << products << " piece(s), storage: " << storage << " piece(s)\n";
            if (debug)
                std::cout << "Producer " << thread_id << " delivered " << products << " piece(s), storage: " << storage << " piece(s)\n";
            storageFileOut << storage;
        }
        else
        {
            logFile << "Producer " << thread_id << " failed to deliver " << products << " piece(s), storage: " << storage << " piece(s)\n";
            localLogFile << "I failed to deliver " << products << " piece(s), storage: " << storage << " piece(s)\n";
            if (debug)
                logFile << "Producer " << thread_id << " failed to deliver " << products << " piece(s), storage: " << storage << " piece(s)\n";
        }
        storageFileOut.close();
        logFile.close();
        localLogFile.close();
        sleep(1);
        if (debug)
            std::cout << "Producer " << thread_id << " left the critical section\n";

        sem_post(&criticalSectionSemaphore);
        sem_post(&consumerSemaphore);
        sleep(1);
    }

    return NULL;
}

void *consumer(void *arg)
{
    long thread_id = (long)arg;

    while (1)
    {
        sem_wait(&consumerSemaphore);
        sem_wait(&criticalSectionSemaphore);

        if (debug)
            std::cout << "Consumer " << thread_id << " entered the critical section\n";
        std::ifstream storageFileIn(storageFileName);
        storageFileIn >> storage;
        storageFileIn.close();
        products = getRandomInt(a, b);
        std::ofstream storageFileOut(storageFileName);
        std::fstream logFile("log.txt", std::ios::app);
        std::string fileName = "consumer" + std::to_string(thread_id) + "log.txt";
        std::fstream localLogFile(fileName, std::ios::app);
        if (products <= storage)
        {
            storage -= products;
            logFile << "Consumer " << thread_id << " picked " << products << " piece(s), storage: " << storage << " piece(s)\n";
            localLogFile << "I picked " << products << " piece(s), storage: " << storage << " piece(s)\n";
            if (debug)
                std::cout << "Consumer " << thread_id << " picked " << products << " piece(s), storage: " << storage << " piece(s)\n";
            storageFileOut << storage;
        }
        else
        {
            logFile << "Consumer " << thread_id << " failed to pick " << products << " piece(s), storage: " << storage << " piece(s)\n";
            localLogFile << "I failed to pick " << products << " piece(s), storage: " << storage << " piece(s)\n";
            if (debug)
                std::cout << "Consumer " << thread_id << " failed to pick " << products << " piece(s), storage: " << storage << " piece(s)\n";
        }
        storageFileOut.close();
        logFile.close();
        localLogFile.close();
        sleep(1);
        if (debug)
            std::cout << "Consumer " << thread_id << " left the critical section\n";

        sem_post(&criticalSectionSemaphore);
        sem_post(&producerSemaphore);
        sleep(1);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc >= 2)
    {
        try
        {
            k = std::stoi(argv[1]);
            n = std::stoi(argv[2]);
            m = std::stoi(argv[3]);
            a = std::stoi(argv[4]);
            b = std::stoi(argv[5]);
            c = std::stoi(argv[6]);
            d = std::stoi(argv[7]);
            paramsSuccess = true;
        }
        catch (...)
        {
            paramsSuccess = false;
            std::cerr << "Warning: Failed to fetch paremeters from the command line. Executing with parameters provided in the params file\n";
        }
    }

    if (!paramsSuccess)
    {
        try
        {
            std::ifstream paramsFile("params.soi_homework_3_custom_extension");
            paramsFile >> k >> n >> m >> a >> b >> c >> d;
            paramsFile.close();
            paramsSuccess = true;
        }
        catch (...)
        {
            paramsSuccess = false;
            std::cerr << "Warning: Failed to fetch parameters from the params file. Fetching default parameters\n";
        }
    }

    if (!paramsSuccess)
    {
        k = 50;
        n = 4;
        m = 7;
        a = 1;
        b = 15;
        c = 1;
        d = 15;
    }

    if (k < 0)
    {
        std::cerr << "Error: Storage size cannot be negative\n";
        return 1;
    }
    if (k > 999999999)
    {
        std::cerr << "Error: Storage size must be lower than 1 billion\n";
        return 1;
    }
    if (n < 1)
    {
        std::cerr << "Error: There must be at least one consumer\n";
        return 1;
    }
    if (n > 999999999)
    {
        std::cerr << "Error: Consumers' count must be lower than 1 billion\n";
        return 1;
    }
    if (m < 1)
    {
        std::cerr << "Error: There must be at least one producer\n";
        return 1;
    }
    if (m > 999999999)
    {
        std::cerr << "Error: Producers' count must be lower than 1 billion\n";
        return 1;
    }
    if (a < 0)
    {
        std::cerr << "Error: Lower bound of consumer's demand cannot be negative\n";
        return 1;
    }
    if (a > 999999999)
    {
        std::cerr << "Error: Lower bound of consumer's demand must be lower than 1 billion\n";
        return 1;
    }
    if (b < 0)
    {
        std::cerr << "Error: Upper bound of consumer's demand cannot be negative\n";
        return 1;
    }
    if (b > 999999999)
    {
        std::cerr << "Error: Upper bound of consumer's demand must be lower than 1 billion\n";
        return 1;
    }
    if (a > b)
    {
        std::cerr << "Error: Lower bound of consumer's demand cannot be higher than upper bound\n";
        return 1;
    }
    if (c < 0)
    {
        std::cerr << "Error: Lower bound of producer's supply cannot be negative\n";
        return 1;
    }
    if (c > 999999999)
    {
        std::cerr << "Error: Lower bound of producer's supply must be lower than 1 billion\n";
        return 1;
    }
    if (d < 0)
    {
        std::cerr << "Error: Upper bound of producer's supply cannot be negative\n";
        return 1;
    }
    if (d > 999999999)
    {
        std::cerr << "Error: Upper bound of producer's supply must be lower than 1 billion\n";
        return 1;
    }
    if (c > d)
    {
        std::cerr << "Error: Lower bound of producer's supply cannot be higher than upper bound\n";
        return 1;
    }

    std::ofstream storageFile(storageFileName);
    storageFile << std::round(k / 2);
    storageFile.close();
    std::ofstream logFile("log.txt", std::ios::trunc);
    logFile.close();
    std::ofstream logFile2("log.txt");
    logFile2 << "Storage at the beginning: " << std::round(k / 2) << " piece(s)\n";
    logFile2.close();

    for (int i = 1; i <= m; i++)
    {
        std::string fileName = "producer" + std::to_string(i) + "log.txt";
        std::ofstream localLogFile(fileName, std::ios::trunc);
        localLogFile.close();
    }
    for (int i = 1; i <= n; i++)
    {
        std::string fileName = "consumer" + std::to_string(i) + "log.txt";
        std::ofstream localLogFile(fileName, std::ios::trunc);
        localLogFile.close();
    }

    sem_init(&producerSemaphore, 0, 1);
    sem_init(&consumerSemaphore, 0, 0);
    sem_init(&criticalSectionSemaphore, 0, 1);

    pthread_t producerThreads[m];
    pthread_t consumerThreads[n];
    for (long i = 1; i <= m; ++i)
        pthread_create(&producerThreads[i - 1], NULL, producer, (void *)i);
    for (long i = 1; i <= n; ++i)
        pthread_create(&consumerThreads[i - 1], NULL, consumer, (void *)i);
    for (int i = 0; i < m; ++i)
        pthread_join(producerThreads[i], NULL);
    for (int i = 0; i < n; ++i)
        pthread_join(consumerThreads[i], NULL);

    sem_destroy(&producerSemaphore);
    sem_destroy(&consumerSemaphore);
    sem_destroy(&criticalSectionSemaphore);

    return 0;
}
