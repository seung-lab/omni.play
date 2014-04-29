#pragma once

#include "precomp.h"
#include "common/macro.hpp"
#include "common/string.hpp"

namespace om {
namespace network {

class Uri {
 public:
  Uri() : Uri("") {}
  explicit Uri(std::string path) : Uri("file", path) {}
  Uri(std::string scheme, std::string path, std::string hostname = "",
      uint8_t port = 0, std::string userinfo = "")
      : scheme_(scheme),
        userinfo_(userinfo),
        hostname_(hostname),
        port_(port),
        path_(path) {}

  void AddQueryParameter(std::string key, std::string value) {
    queryParameters_.push_back(std::make_pair(key, value));
  }

  void ClearQueryParameters() { queryParameters_.clear(); }

  static boost::optional<Uri> Parse(const std::string& fullUri) {
    Uri result;
    // from here:
    // http://snipplr.com/view/6889/regular-expressions-for-uri-validationparsing/
    //- groups are as follows:
    //  1   == scheme
    //  2   == authority
    //  4   == userinfo
    //  5   == host (loose check to allow for IPv6 addresses)
    //  6   == port
    //  7,9 == path (7 if it has an authority, 9 if it doesn't)
    //  11   == query
    //  12   == fragment
    boost::regex r(
        "^"
        "([a-z][a-z0-9+.-]*):"  // 1 scheme
        "(?:"
        "//"  // it has an authority:
        "("   // 2 authority
              // 4 userinfo
        "(?:(?=((?:[a-z0-9\\-._~!$&'()*+,;=:]|%[0-9A-F]{2})*))(\\3)@)?"
        "(?=(\\[[0-9A-F:.]{2,}\\]|(?:[a-z0-9\\-._~!$&'()*+,;=]|%[0-9A-F]{2}"
        ")*))"
        "\\5"  // 5 host (loose check to allow for IPv6 addresses)
        "(?::(?=(\\d*))\\6)?"  // 6 port
        ")"
        // 7 path
        "(/(?=((?:[a-z0-9\\-._~!$&'()*+,;=:@/]|%[0-9A-F]{2})*))\\8)?"
        "|"  // it doesn't have an authority:
        "(/?(?!/)(?=((?:[a-z0-9\\-._~!$&'()*+,;=:@/"
        "]|%[0-9A-F]{2})*))\\10)?"  // 9 path
        ")"
        "(?:"
        // 11 query string
        "\\?(?=((?:[a-z0-9\\-._~!$&'()*+,;=:@/?]|%[0-9A-F]{2})*))\\11"
        ")?"
        "(?:"
        // 12 fragment
        "#(?=((?:[a-z0-9\\-._~!$&'()*+,;=:@/?]|%[0-9A-F]{2})*))\\12"
        ")?");

    boost::match_results<std::string::const_iterator> mr;
    if (!boost::regex_match(fullUri, mr, r)) {
      return boost::optional<Uri>();
    }

    if (mr[1].matched) {
      result.set_scheme(mr[1].str());
    } else {
      result.set_scheme("http");
    }

    if (mr[4].matched) {
      result.set_userinfo(mr[4].str());
    }

    if (mr[5].matched) {
      result.set_hostname(mr[5].str());
    }

    if (mr[6].matched) {
      result.set_port(std::stoi(mr[6].str()));
    }

    if (mr[7].matched) {
      result.set_path(mr[7].str());
    }

    if (mr[11].matched) {
      auto pairs = string::split(mr[11].str(), '&');
      for (auto& p : pairs) {
        auto split = p.find_first_of('=');
        if (split != std::string::npos) {
          result.AddQueryParameter(p.substr(0, split), p.substr(split + 1));
        }
      }
    }

    if (mr[12].matched) {
      result.set_fragment(mr[12].str());
    }
    return result;
  }

  void AppendPath(std::string rel) { path_ += rel; }

  // TODO: Sanitize inputs to make sure there are no characters which can't be
  // encoded in uris. urlencode
  // TODO: Make sure the resulting URI isn't too long (2k chars)
  // TODO: Canonicalize paths (get rid of // etc.)

  std::string string() const {
    std::stringstream ss;

    ss << scheme_ << "://";
    if (userinfo_.size()) {
      ss << userinfo_ << "@";
    }
    if (hostname_.size()) {
      ss << hostname_;
    }
    if (port_) {
      ss << ":" << (uint32_t)port_;
    }
    ss << path_;
    if (queryParameters_.size()) {
      ss << "?";
      for (size_t i = 0; i < queryParameters_.size(); ++i) {
        if (i) {
          ss << "&";
        }
        ss << queryParameters_[i].first << "=" << queryParameters_[i].second;
      }
    }

    if (fragment_.size()) {
      ss << "#" << fragment_;
    }
    return ss.str();
  }

 private:
  PROP_REF_SET(std::string, scheme);
  PROP_REF_SET(std::string, userinfo);
  PROP_REF_SET(std::string, hostname);
  PROP_REF_SET(uint8_t, port);
  PROP_REF_SET(std::string, path);

  typedef std::vector<std::pair<std::string, std::string>> ParameterList;
  PROP_CONST_REF(ParameterList, queryParameters);
  PROP_REF_SET(std::string, fragment);
};

inline std::ostream& operator<<(std::ostream& out, const Uri& uri) {
  return out << uri.string();
}
}
}  // namespace om::network::
