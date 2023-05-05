#include <iostream>
#include <vector>
#include <Windows.h>
#include <fstream>

struct Message {
    int size;
    std::string body;
};

class Mailbox {
private:
    int max_size_;
    int num_messages_;
    int total_size_;
    std::vector<Message> messages_;
    std::wstring mailbox_file_;

    void saveMailbox() {
        std::ofstream outfile(mailbox_file_, std::ios::binary);
        if (!outfile.is_open()) {
            std::cout << "Error opening mailbox file." << std::endl;
            return;
        }

        for (Message message : messages_) {
            outfile.write(reinterpret_cast<char*>(&message.size), sizeof(int));
            outfile.write(message.body.c_str(), message.size);
        }

        outfile.close();
    }

    void loadMailbox() {
        std::ifstream infile(mailbox_file_, std::ios::binary);
        if (!infile.is_open()) {
            std::cout << "Mailbox file not found. Creating new mailbox file." << std::endl;
            return;
        }

        infile.seekg(0, std::ios::end);
        std::streampos file_size = infile.tellg();
        infile.seekg(0, std::ios::beg);

        while (infile.tellg() < file_size) {
            Message message;
            infile.read(reinterpret_cast<char*>(&message.size), sizeof(int));
            char* buffer = new char[message.size];
            infile.read(buffer, message.size);
            message.body = std::string(buffer, message.size);
            delete[] buffer;
            addMessage(message);
        }

        infile.close();
    }

public:
    Mailbox(int max_size, std::wstring mailbox_file) : max_size_(max_size), num_messages_(0), total_size_(0), mailbox_file_(mailbox_file) {
        loadMailbox();
    }

    ~Mailbox() {
        saveMailbox();
    }

    void addMessage(Message message) {
        if (total_size_ + message.size <= max_size_) {
            messages_.push_back(message);
            num_messages_++;
            total_size_ += message.size;
        }
    }

    Message readMessage(int index, bool delete_message = false) {
        Message message = messages_[index];
        if (delete_message) {
            messages_.erase(messages_.begin() + index);
            num_messages_--;
            total_size_ -= message.size;
        }
        return message;
    }

    int getNumMessages() const {
        return num_messages_;
    }

    int getTotalSize() const {
        return total_size_;
    }

    int getMaxSize() const {
        return max_size_;
    }
};

int main() {
    std::wstring mailbox_file = L"mailbox.bin";
    Mailbox mailbox(1000, mailbox_file);

    while (true) {
        std::cout << "Mailbox Menu:" << std::endl;
        std::cout << "1. Add message" << std::endl;
        std::cout << "2. Read message" << std::endl;
        std::cout << "3. Delete message" << std::endl;
        std::cout << "4. Show number of messages" << std::endl;
        std::cout << "5. Show total size of messages" << std::endl;
        std::cout << "6. Exit" << std::endl;

        int choice;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == 1) {
            std::cout << "Enter message size (in bytes): ";
            int size;
            std::cin >> size;

            std::cout << "Enter message body: ";
            std::string body;

            Message message = { size, body };
            mailbox.addMessage(message);

            std::cout << "Message added to mailbox." << std::endl;
        }
        else if (choice == 2) {
            if (mailbox.getNumMessages() == 0) {
                std::cout << "Mailbox is empty." << std::endl;
            }
            else {
                std::cout << "Enter message index: ";
                int index;
                std::cin >> index;

                if (index < 0 || index >= mailbox.getNumMessages()) {
                    std::cout << "Invalid message index." << std::endl;
                }
                else {
                    Message message = mailbox.readMessage(index);
                    std::cout << "Message body:" << std::endl;
                    std::cout << message.body << std::endl;
                }
            }
        }
        else if (choice == 3) {
            if (mailbox.getNumMessages() == 0) {
                std::cout << "Mailbox is empty." << std::endl;
            }
            else {
                std::cout << "Enter message index: ";
                int index;
                std::cin >> index;

                if (index < 0 || index >= mailbox.getNumMessages()) {
                    std::cout << "Invalid message index." << std::endl;
                }
                else {
                    Message message = mailbox.readMessage(index, true);
                    std::cout << "Message deleted." << std::endl;
                }
            }
        }
        else if (choice == 4) {
            std::cout << "Number of messages: " << mailbox.getNumMessages() << std::endl;
        }
        else if (choice == 5) {
            std::cout << "Total size of messages (in bytes): " << mailbox.getTotalSize() << std::endl;
        }
        else if (choice == 6) {
            std::cout << "Exiting mailbox program." << std::endl;
            break;
        }
        else {
            std::cout << "Invalid choice. Please try again." << std::endl;
        }

        std::cout << std::endl;
    }

    return 0;
}