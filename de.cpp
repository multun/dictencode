#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <gmpxx.h>

// www.cs.uvic.ca/~ruskey/Publications/RankPerm/RankPerm.html

template<class Alpha>
static void unrank1(std::vector<Alpha> &v, mpz_class r)
{
    std::unordered_map<size_t, Alpha> trans_map;
    std::vector<size_t> int_vect;
    for (size_t i = 0; i < v.size(); i++)
    {
        trans_map[i] = v[i];
        int_vect.push_back(i);
    }

    unrank1(int_vect, r);
    for (size_t i = 0; i < v.size(); i++)
        v[i] = trans_map[int_vect[i]];
}

template<>
void unrank1(std::vector<size_t> &v, mpz_class r)
{
    for (size_t n = v.size(); n; n--)
    {
        std::swap(v[n-1], v[mpz_class(r % n).get_ui()]);
        r /= n;
    }
}

static mpz_class rank1(size_t n, std::vector<size_t> &v, std::vector<size_t> &v_i)
{
    if (n == 1)
        return 0;

    size_t s = v[n - 1];
    std::swap(v[n - 1], v[v_i[n - 1]]);
    std::swap(v_i[s], v_i[n - 1]);
    return s + n * rank1(n - 1, v, v_i);
}

static mpz_class rank1(std::vector<size_t> &v)
{
    std::vector<size_t> v_i(v.size());
    for (size_t i = 0; i < v.size(); i++)
        v_i[v[i]] = i;
    return rank1(v.size(), v, v_i);
}


template<class Alpha>
static mpz_class rank1(std::vector<Alpha> &id, std::vector<Alpha> &v)
{
    std::unordered_map<Alpha, size_t> trans_map;
    for (size_t i = 0; i < v.size(); i++)
        trans_map[id[i]] = i;

    std::vector<size_t> int_vect;
    for (size_t i = 0; i < v.size(); i++)
        int_vect.push_back(trans_map[v[i]]);
    return rank1(int_vect);
}

using map_t = std::map<std::string, int>;

bool comp_dict_vals(const map_t::value_type *i, const map_t::value_type *j) {
    const std::string &i_key = i->first;
    const std::string &j_key = j->first;
    return i_key < j_key;
}

static mpz_class mpz_class_fac(size_t n)
{
    mpz_class res;
    mpz_fac_ui(res.get_mpz_t(), n);
    return res;
}

static size_t mpz_class_sizeinbase(const mpz_class& num, size_t base)
{
    return mpz_sizeinbase(num.get_mpz_t(), base);
}

struct bit_stream
{
    bit_stream(uint8_t *data, size_t bit_size)
        : data(data)
        , bit_size(bit_size)
        , cur_bit(0)
    {}

    uint8_t next()
    {
        uint8_t cur_byte = data[cur_bit / 8];
        int offset = cur_bit % 8;
        cur_bit++;
        return (cur_byte >> offset) & 1;
    }

    size_t remaining()
    {
        return bit_size - cur_bit;
    }

private:
    uint8_t *data;
    size_t bit_size;
    size_t cur_bit;
};

size_t perm_available_bits(const mpz_class &permutation_count)
{
    // simple tests here:
    // assert(perm_available_bits(6) == 2);
    // assert(perm_available_bits(7) == 2);
    // assert(perm_available_bits(8) == 3);
    // assert(perm_available_bits(9) == 3);
    // assert(perm_available_bits(10) == 3);
    return mpz_class_sizeinbase(permutation_count, 2) - 1;
}

std::vector<const map_t::value_type*> map_reference(map_t map)
{
    std::vector<const map_t::value_type*> map_vect;
    for(const auto& item : map)
        map_vect.push_back(&item);
    auto sorted_map_vect = map_vect;
    std::sort(sorted_map_vect.begin(), sorted_map_vect.end(), comp_dict_vals);
    return sorted_map_vect;
}

int main(void)
{
    unsigned char data[] = { "coucou" };
    bit_stream test_bit_stream{data, sizeof(data) * 8};

    map_t test_map{};

    for (size_t i = 0; i < 20; i++)
        test_map[std::to_string(i)] = i;

    // build the reference sorted vector
    auto sorted_map_vect = map_reference(test_map);

    // count permutations
    auto permutation_count = mpz_class_fac(sorted_map_vect.size());

    // count how many bits can be stored for the given permutation count
    auto available_bits = perm_available_bits(permutation_count);

    std::cout << "available space: " << available_bits << std::endl;
    std::cout << "pending bits: " << test_bit_stream.remaining() << std::endl;
    mpz_class permutation_id = 0;
    for (size_t i = 0; i < available_bits; i++)
    {
        if (test_bit_stream.remaining() == 0)
            break;

        if (test_bit_stream.next())
            mpz_setbit(permutation_id.get_mpz_t(), i);
    }

    auto map_vect = sorted_map_vect;
    unrank1(map_vect, permutation_id);

    auto rank = rank1(sorted_map_vect, map_vect);

    std::cout << permutation_id << std::endl;
    std::cout << rank << std::endl;
}
