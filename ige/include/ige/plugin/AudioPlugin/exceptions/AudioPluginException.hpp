/*
** EPITECH PROJECT, 2021
** ige
** File description:
** AudioPluginException
*/

#ifndef AUDIOPLUGINEXCEPTION_HPP_
#define AUDIOPLUGINEXCEPTION_HPP_

#include <exception>
#include <string>

namespace ige {
namespace plugin {
    namespace audio {

        class AudioPluginException : public std::exception {
        public:
            explicit AudioPluginException()
                : AudioPluginException("Generic exception")
            {
            }

            explicit AudioPluginException(const std::string& message)
                : msg_(message)
            {
            }

            virtual ~AudioPluginException() noexcept
            {
            }

            virtual const char* what() const noexcept
            {
                return (msg_.c_str());
            }

        protected:
            std::string msg_;

        private:
        };

    }
}
}

#endif /* !AUDIOPLUGINEXCEPTION_HPP_ */
