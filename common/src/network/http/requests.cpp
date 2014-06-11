#include "network/http/requests.h"

namespace om {
namespace network {
namespace http {

#define SET_OPT(h, opt, val)                                           \
  err = curl_easy_setopt(h, opt, val);                                 \
  if (err != CURLE_OK) {                                               \
    log_debugs << "Failed to set " << #opt << curl_easy_strerror(err); \
    return;                                                            \
  }

// Reimplement to avoid copies.
static size_t write_data(char* buffer, size_t size, size_t nmemb,
                         std::ostream* stream) {
  stream->write(buffer, size * nmemb);
  return size * nmemb;
}

// Reimplement to avoid copies.
static size_t read_data(char* buffer, size_t size, size_t nmemb,
                        std::istream* stream) {
  stream->read(buffer, size * nmemb);
  if (stream->eof()) {
    return stream->gcount();
  }
  return size * nmemb;
}

#define OMNI_USER_AGENT "Omni"

void HTTPRequest::SetCurlOptions(CURL* h) {
  CURLcode err;
  SET_OPT(h, CURLOPT_URL, uri_.string().c_str());
  SET_OPT(h, CURLOPT_FOLLOWLOCATION, 1);
  SET_OPT(h, CURLOPT_USERAGENT, OMNI_USER_AGENT);
  SET_OPT(h, CURLOPT_COOKIEJAR, file::Paths::CookieFile().c_str());
  SET_OPT(h, CURLOPT_SSL_VERIFYPEER, 0);
}

void GetRequest::SetCurlOptions(CURL* h) {
  HTTPRequest::SetCurlOptions(h);
  CURLcode err;
  SET_OPT(h, CURLOPT_WRITEFUNCTION, &write_data);
  SET_OPT(h, CURLOPT_WRITEDATA, &ss_);
}

void PutRequest::SetCurlOptions(CURL* h) {
  HTTPRequest::SetCurlOptions(h);
  CURLcode err;
  SET_OPT(h, CURLOPT_READFUNCTION, &read_data);
  SET_OPT(h, CURLOPT_READDATA, &ss_);
  SET_OPT(h, CURLOPT_PUT, 1);
}

void PostRequest::SetCurlOptions(CURL* h) {
  HTTPRequest::SetCurlOptions(h);

  CURLcode err;
  SET_OPT(h, CURLOPT_POST, 1);

  log_debugs << "Posting: " << postString_;
  SET_OPT(h, CURLOPT_POSTFIELDS, postString_.c_str());
  if (postString_.empty()) {
    SET_OPT(h, CURLOPT_POSTFIELDSIZE, 0);
  }

  SET_OPT(h, CURLOPT_WRITEFUNCTION, &write_data);
  SET_OPT(h, CURLOPT_WRITEDATA, &ss_);
}

void PostRequest::Finish(CURL* h) {
  if (200 <= returnCode_ && returnCode_ < 300) {
    result_ = ss_.str();
  }
  do_continuation(result_);
}
}
}
}