#include <gigamonkey/schema/hd.hpp>
#include <data/crypto/NIST_DRBG.hpp>
#include <data/crypto/secret_share.hpp>
#include <data/encoding/ascii.hpp>
#include <data/io/exception.hpp>

std::ostream inline &operator<<(std::ostream &o, const data::crypto::secret_share &x) {
    return o << Gigamonkey::base58::check(static_cast<data::byte>(x.Index), x.Data).encode();
}

namespace Gigamonkey {
    
    string secret_share_split(list<string> args) {
        
        if (args.size() != 3) 
            throw exception("3 further arguments required for split");
        
        string message = args[0];
        if (!encoding::ascii::valid(message)) 
            throw exception("message must be an ascii string");
        
        uint64 shares;
        {
            std::stringstream ss(args[1]);
            ss >> shares;
        }
        
        if (shares > 255 || shares == 0) 
            throw exception{} << "argument 3 (shares) must be less than 256 and greater than zero; " << shares;
        
        uint64 threshold;
        {
            std::stringstream ss(args[2]);
            ss >> threshold;
        }
        
        if (threshold == 0 || threshold > shares) 
            throw exception("argument 4 (threshold) must be greater than zero and less than shares.");
        
        ptr<crypto::entropy> entropy = std::static_pointer_cast<crypto::entropy>(std::make_shared<crypto::user_entropy>(
            "Please seed random number generator with entropy.", "Entropy accepted", std::cout, std::cin));
        
        crypto::NIST::DRBG random{crypto::NIST::DRBG::HMAC_DRBG, entropy, bytes{}, 302};
        
        cross<crypto::secret_share> secret_shares = 
            crypto::secret_share_split(*random.Random, bytes::from_string(message), 
                static_cast<byte>(shares), static_cast<byte>(threshold));
        
        list<string> output; 
        for (const crypto::secret_share &x : secret_shares) {
            output = output << base58::check(static_cast<byte>(x.Index), x.Data).encode();
        }
        
        std::stringstream ss;
        ss << output;
        return ss.str();
        
    }
    
    string secret_share_merge(list<string> args) {
        
        if (!data::encoding::decimal::valid(args.first())) 
            throw exception("First argument must be a decimal number between from 1 to 255.");
    
        uint64 threshold;
        {
            std::stringstream ss(args[0]);
            ss >> threshold;
        }
        
        if (threshold == 0) 
            throw exception("threshold must be greater than zero");
        
        if (threshold > args.size() - 1) 
            throw exception("must provide at least as many additional arguments as the threshold.");
        
        if (threshold > 255) 
            throw exception("Maximum threshold value is 255.");
        
        cross<crypto::secret_share> shares(args.size() - 1);
        
        int i = 0;
        for (const string& x : args.rest()) {
            base58::check share = base58::check::recover(x);
            if (!share.valid()) throw exception{} << "could not recover share " << i;
            string encoded = share.encode();
            if (encoded != x) std::cout << "recovered base 58 check string " << encoded << " from " << x << std::endl;
            shares[i] = crypto::secret_share{share.version(), share.payload()};
            i ++;
        }
        
        bytes merged = crypto::secret_share_merge(shares, static_cast<byte>(threshold));
        
        string merged_string = encoding::ascii::write(merged);
        if (!encoding::ascii::valid(merged_string)) 
            throw exception{} << "invalid ascii string recovered; here it is in hex: " << encoding::hex::write(merged);
        
        return merged_string;
        
    }
    
    string help() {
        return 
            "\thelp                                                  Show his message.\n"
            "\tversion                                               Show version.\n"
            "\tsplit <data:string> <shares:uint> <threshold:uint>    Split data into shares.\n"
            "\tmerge <threshold:uint> <shares:string...>             Merge shares into data.";
    }
    
    string version() {
        return "1.1";
    }
    
    string secret_share(list<string> args) {
        
        if (args.empty() || args.first() == string{"help"}) return help();
        
        if (args.first() == string{"version"}) return version();
        
        if (args.first() == string{"split"}) return secret_share_split(args.rest());
        
        if (args.first() == string{"merge"}) return secret_share_merge(args.rest());
        
        return help();
        
    }
    
}

int main(int num_args, char** arg) {
    
    using namespace Gigamonkey;
    
    list<string> args;
    for (int i = 1; i < num_args; i++) {
        args = args << string{arg[i]};
    }
    
    try {
        
        std::cout << secret_share(args) << std::endl;
        
    } catch (const exception& ex) {
        
        std::cout << "Error: " << ex.message() << std::endl;
        
        return 1;
    }
    
    return 0;
    
}

