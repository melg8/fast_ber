#include "autogen/pokemon.hpp"

#include <fstream>
#include <iostream>
#include <vector>

int main()
{
    std::ifstream input("pokemon.ber", std::ios::binary);
    if (!input.good())
    {
        std::cout << "Failed to open input file: pokemon.ber\n";
        return -1;
    }
    const auto buffer = std::vector<uint8_t>{std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};

    fast_ber::Pokemon::Team team;
    if (!fast_ber::Pokemon::decode(absl::MakeSpan(buffer.data(), buffer.size()), team, 0))
    {
        std::cout << "Failed to decode input file: pokemon.ber\n";
        return -1;
    }

    for (const auto& member : team.members)
    {
        std::cout << "Pokemon = " << member.name << '\n';
        std::cout << "Category = " << member.ability << '\n';
        std::cout << "Type = " << member.type << '\n';
        std::cout << "Ability = " << member.ability << '\n';
        std::cout << "Weakness = " << member.weakness << '\n';
        std::cout << "Weight = " << member.weight << '\n';
        std::cout << "\n";
    }
    return 0;
}
