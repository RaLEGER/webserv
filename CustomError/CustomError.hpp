#ifndef CUSTOMERROR_HPP
#define CUSTOMERROR_HPP

#include <stdexcept>
#include <string>

class CustomError : public std::exception {
    public:
        CustomError(int errorCode, const char* message = "")
            : errorCode(errorCode), customMessage(message) {}

        virtual ~CustomError() throw() {}

        virtual const char* what() const throw() {
            return customMessage.empty() ? "Custom exception occurred" : customMessage.c_str();
        }

        int getErrorCode() const {
            return errorCode;
        }

    private:
        int errorCode;
        std::string customMessage;
};

#endif // MYEXCEPTION_HPP
