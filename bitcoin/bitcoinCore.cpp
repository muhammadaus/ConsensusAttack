#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <string>
#include <sstream>
#include <mutex>
#include <ctime>

class Block {
public:
    int index;
    std::string previousHash;
    std::time_t timestamp;
    std::string data;
    std::string hash;

    Block(int idx, const std::string& prevHash, const std::string& data) 
        : index(idx), previousHash(prevHash), data(data) {
        timestamp = std::time(nullptr);
        hash = calculateHash();
    }

    std::string calculateHash() {
        std::stringstream ss;
        ss << index << previousHash << timestamp << data;
        return std::to_string(std::hash<std::string>{}(ss.str()));
    }
};

class Blockchain {
private:
    std::vector<Block> chain;
    std::mutex mtx; // Mutex for thread safety

public:
    Blockchain() {
        createGenesisBlock();
    }

    void createGenesisBlock() {
        Block genesisBlock(0, "0", "Genesis Block");
        chain.push_back(genesisBlock);
    }

    void addBlock(const std::string& data) {
        std::lock_guard<std::mutex> lock(mtx); // Lock for thread safety
        Block newBlock(chain.size(), chain.back().hash, data);
        
        // Simulate random mining success
        int randomValue = rand() % 100; // Random number between 0 and 99
        std::cout << "Node attempting to mine block with random value: " << randomValue << std::endl;

        // Assume any random value below 50 means successful mining
        if (randomValue < 50) {
            std::cout << "Block mined successfully!" << std::endl;
            chain.push_back(newBlock);
            std::cout << "Block " << newBlock.index << " added: " << newBlock.data 
                      << " | Hash: " << newBlock.hash << std::endl;
        } else {
            std::cout << "Mining failed, trying again..." << std::endl;
        }
    }
};

void nodeFunction(Blockchain& blockchain, int nodeId) {
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, 5);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(distribution(generator))); // Simulate random block creation time
        blockchain.addBlock("Data from node " + std::to_string(nodeId));
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0))); // Seed for random number generation
    Blockchain blockchain;
    std::vector<std::thread> nodes;

    // Create multiple nodes
    for (int i = 0; i < 5; ++i) { // 5 nodes
        nodes.emplace_back(nodeFunction, std::ref(blockchain), i);
    }

    // Keep the main thread alive
    for (auto& node : nodes) {
        node.join();
    }

    return 0;
}