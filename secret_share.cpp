

#include <data/crypto/secret_share.hpp>
#include <data/encoding/ascii.hpp>
#include <data/io/random.hpp>
#include <data/io/main.hpp>

#include <gigamonkey/p2p/checksum.hpp>

namespace Gigamonkey {
    std::string secret_share (std::span<const char *const> args);
}

using error = data::io::error;

namespace data {
    void signal_handler (int signal) noexcept {
        exit (0);
    }

    error main (std::span<const char *const> args) {

        using namespace Gigamonkey;

        try {

            DATA_LOG (normal) << secret_share (args);

        } catch (const exception &ex) {

            std::cout << "Error: " << ex.what () << std::endl;

            return error::code {ex.Code};
        }

        return error {};

    }
}

std::ostream inline &operator << (std::ostream &o, const data::crypto::secret_share &x) {
    return o << Gigamonkey::base58::check (static_cast<data::byte> (x.Index), x.Data).encode ();
}

namespace data::random {
    bytes Personalization {string ("Secret Share v1.5 noop nyuup zadooop!! ##)))")};
}

namespace Gigamonkey {
    
    std::string secret_share_split (std::span<const char *const> args);
    
    std::string secret_share_merge (std::span<const char *const> args);

    std::string help ();

    std::string version ();
    
    std::string secret_share (std::span<const char *const> args) {

        if (args.empty () || args[0] == "help") return help ();

        if (args[0] == "version") return version ();

        if (args[0] == "split") return secret_share_split (args.last (args.size () - 1));

        if (args[0] == "merge") return secret_share_merge (args.last (args.size () - 1));

        return help ();

    }

    std::string secret_share_split (std::span<const char *const> args) {

        if (args.size () != 3)
            throw exception (error::user_action) << "3 further arguments required for split";

        string message = args[0];

        uint64 shares;
        {
            std::stringstream ss (args[1]);
            ss >> shares;
        }

        if (shares > 10 || shares == 0)
            throw exception (error::user_action) << "argument 3 (shares) must be less than 10 and greater than zero; " << shares;

        uint64 threshold;
        {
            std::stringstream ss (args[2]);
            ss >> threshold;
        }

        if (threshold == 0 || threshold > shares)
            throw exception (error::user_action) << "argument 4 (threshold) must be greater than zero and less than shares.";

        data::random::init ({.strength = 256});

        cross<crypto::secret_share> secret_shares =
        crypto::secret_share_split (crypto::random::get (), bytes (message), static_cast<uint32> (shares), static_cast<uint32> (threshold));

        list<string> output;
        for (const crypto::secret_share &x : secret_shares)
            output = output << base58::check (static_cast<byte> (x.Index), x.Data).encode ();

        std::stringstream ss;
        ss << output;
        return ss.str ();

    }

    std::string secret_share_merge (std::span<const char *const> args) {

        if (args.size () == 0)
            throw exception (error::user_action) << "no further arguments after 'merge'. First we need the number of shares, followed by shares.";

        maybe<data::N_bytes_little> threshold_arg = encoding::decimal::read<data::endian::little, byte> (args[0]);

        uint64 threshold;
        {
            std::stringstream ss (args[0]);
            ss >> threshold;
        }

        if (threshold == 0 || threshold > args.size () - 1)
            throw exception (error::user_action) <<
            "threshold must be greater than zero and the number of additional arguments provided must be at least the threshold.";

        cross<crypto::secret_share> shares (args.size () - 1);

        int i = 0;
        for (const string &x : args.last (args.size () - 1)) {
            base58::check share = base58::check::recover (x);
            if (!share.valid ()) throw exception (error::user_action) << "could not recover share " << i;
            string encoded = share.encode ();
            if (encoded != x) DATA_LOG (normal) << "recovered base 58 check string " << encoded << " from " << x << std::endl;
            shares[i] = crypto::secret_share{share.version (), share.payload ()};
            i ++;
        }

        bytes merged = crypto::secret_share_merge (shares, static_cast<byte> (threshold));

        data::ASCII merged_string {merged};
        // valid ASCII has no negative chars
        if (!merged_string.valid ())
            throw exception (1) << "invalid string recovered; here it is in hex: " << encoding::hex::write (merged);

        // add quotes
        std::stringstream ss;
        ss << merged_string;
        return ss.str ();

    }

    std::string help () {
        return
        "\thelp                                                  Show his message.\n"
        "\tversion                                               Show version.\n"
        "\tsplit <data:string> <shares:uint> <threshold:uint>    Split data into shares.\n"
        "\tmerge <threshold:uint> <shares:string...>             Merge shares into data.";
    }

    std::string version () {
        return "1.2";
    }
}
