namespace internal {
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

static void GetOptions(CURL* h, const network::Uri& uri,
                       std::stringstream* ss) {
  CURLcode err;
  SET_OPT(h, CURLOPT_URL, uri.string().c_str());
  SET_OPT(h, CURLOPT_FOLLOWLOCATION, 1);
  SET_OPT(h, CURLOPT_WRITEFUNCTION, &write_data);
  SET_OPT(h, CURLOPT_USERAGENT, OMNI_USER_AGENT);

  SET_OPT(h, CURLOPT_WRITEDATA, ss);
  SET_OPT(h, CURLOPT_COOKIEJAR, file::Paths::CookieFile().c_str());
  SET_OPT(h, CURLOPT_SSL_VERIFYPEER, 0);
}

static void PutOptions(CURL* h, const network::Uri& uri,
                       std::stringstream* ss) {
  CURLcode err;
  SET_OPT(h, CURLOPT_URL, uri.string().c_str());
  SET_OPT(h, CURLOPT_FOLLOWLOCATION, 1);
  SET_OPT(h, CURLOPT_USERAGENT, OMNI_USER_AGENT);

  SET_OPT(h, CURLOPT_READFUNCTION, &read_data);
  SET_OPT(h, CURLOPT_READDATA, ss);
  SET_OPT(h, CURLOPT_COOKIEJAR, file::Paths::CookieFile().c_str());
  SET_OPT(h, CURLOPT_SSL_VERIFYPEER, 0);
  SET_OPT(h, CURLOPT_PUT, 1);
}

static void PostOptions(CURL* h, const network::Uri& uri, std::stringstream* ss,
                        const std::string& post) {
  CURLcode err;
  SET_OPT(h, CURLOPT_URL, uri.string().c_str());
  SET_OPT(h, CURLOPT_FOLLOWLOCATION, 1);
  SET_OPT(h, CURLOPT_USERAGENT, OMNI_USER_AGENT);

  SET_OPT(h, CURLOPT_POST, 1);

  log_debugs << "Posting: " << post;
  SET_OPT(h, CURLOPT_POSTFIELDS, post.c_str());
  if (post.empty()) {
    SET_OPT(h, CURLOPT_POSTFIELDSIZE, 0);
  }

  SET_OPT(h, CURLOPT_WRITEFUNCTION, &write_data);
  SET_OPT(h, CURLOPT_WRITEDATA, ss);

  SET_OPT(h, CURLOPT_COOKIEJAR, file::Paths::CookieFile().c_str());
  SET_OPT(h, CURLOPT_SSL_VERIFYPEER, 0);
}

template <typename TKey, typename TVal, typename... TRest>
static std::string postString(std::pair<TKey, TVal> toPost, TRest... rest) {
  return postPair(url_encode(toPost.first), toPost.second) + "&" +
         postString(rest...);
}
template <typename TKey, typename TVal>
static std::string postString(std::pair<TKey, TVal> toPost) {
  return postPair(url_encode(toPost.first), toPost.second);
}
static std::string postString() { return ""; }
static std::string postString(const file::path& p) {
  return "@" + boost::filesystem::absolute(p).string();
}

template <typename TVal>
static std::string postPair(std::string first, TVal second) {
  return first + "=" + url_encode(second);
}

template <typename TKey, typename TVal>
static std::string postPair(const std::string& name, std::map<TKey, TVal> map) {
  std::string result;
  bool first = true;
  for (auto& pair : map) {
    if (!first) {
      result += "&";
    } else {
      first = false;
    }
    result += postPair(std::string(name) + "[" + url_encode(pair.first) + "]",
                       pair.second);
  }
  return result;
}

template <typename TKey, typename TVal>
static std::string postPair(const std::string& name,
                            std::unordered_map<TKey, TVal> map) {
  std::string result;
  bool first = true;
  for (auto& pair : map) {
    if (!first) {
      result += "&";
    } else {
      first = false;
    }
    result += postPair(std::string(name) + "[" + url_encode(pair.first) + "]",
                       pair.second);
  }
  return result;
}

template <typename T>
static std::string postPair(const std::string& name, std::vector<T> vec) {
  std::string result;
  bool first = true;
  for (auto& elem : vec) {
    if (!first) {
      result += "&";
    } else {
      first = false;
    }
    result += postPair(std::string(name) + "[]", elem);
  }
  return result;
}

static std::string url_encode(const char* str) {
  char* encoded = curl_easy_escape(nullptr, str, 0);
  std::string result(encoded);
  curl_free(encoded);
  return result;
}
static std::string url_encode(std::string val) {
  return url_encode(val.c_str());
}
template <typename T>
static std::string url_encode(T val) {
  return url_encode(std::to_string(val).c_str());
}
}  // end internal
