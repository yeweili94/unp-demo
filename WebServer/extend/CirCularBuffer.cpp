// #include <WebServer/CirCularBuffer.h>
#include "./CirCularBuffer.h"

using namespace ywl;
using namespace ywl::net;

const char Buffer::kCRLF[] = "\r\n";
const size_t Buffer::KInitialSize = 1024;
const size_t Buffer::ReservedPrependSize = 8;

ssize_t Buffer::readFd(int sockfd, int *savedErrno)
{
    char extrabuf[1024*64];
    struct iovec vec[2];
    // const size_t writable = writeableBytes();
    size_t writable;
    if (writerIndex_ > readerIndex_) {
        writable = capacity_ - writerIndex_;
    }
    else {
        writable = writeableBytes();
    }

    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;

    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    const ssize_t n = ::readv(sockfd, vec, 2);

    if (n < 0)
    {
        *savedErrno = errno;
    }
    else if (static_cast<size_t>(n) <= writable)
    {
        writerIndex_ += n;
        if (writerIndex_ == capacity_) {
            writerIndex_ = ReservedPrependSize;
        }
    }
    else
    {
        writerIndex_ = reserved_prepend_size_;
        append(extrabuf, n - writable);
    }
    return n;
}
