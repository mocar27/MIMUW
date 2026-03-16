#ifndef ORGANISM_H
#define ORGANISM_H

#include <optional>
#include <tuple>
#include <string>

template<typename species_t, bool can_eat_meat, bool can_eat_plants> requires std::equality_comparable<species_t>
class Organism {
private:
    uint64_t vitality;
    const species_t type;

public:
    constexpr Organism(species_t const &species, uint64_t vitality) :
            vitality(vitality),
            type(species) {}

    constexpr uint64_t get_vitality() const {
        return vitality;
    }

    constexpr const species_t &get_species() const {
        return type;
    }

    constexpr bool is_dead() const {
        return vitality == 0;
    }
};

/* Rozmnażanie dwóch organizmów */
template<typename species_t, bool sp_eats_m, bool sp_eats_p>
constexpr std::tuple<Organism<species_t, sp_eats_m, sp_eats_p>,
        Organism<species_t, sp_eats_m, sp_eats_p>,
        std::optional<Organism<species_t, sp_eats_m, sp_eats_p>>>
mating(Organism<species_t, sp_eats_m, sp_eats_p> organism1,
       Organism<species_t, sp_eats_m, sp_eats_p> organism2) {

    uint64_t modulo = organism1.get_vitality() % 2 + organism2.get_vitality() % 2;
    uint64_t vitality = (organism1.get_vitality() / 2 + organism2.get_vitality() / 2 + modulo / 2);

    Organism<species_t, sp_eats_m, sp_eats_p> newOrganism(organism1.get_species(), vitality);
    std::optional<Organism<species_t, sp_eats_m, sp_eats_p>> opt = newOrganism;

    std::tuple<Organism<species_t, sp_eats_m, sp_eats_p>,
            Organism<species_t, sp_eats_m, sp_eats_p>,
            std::optional<Organism<species_t, sp_eats_m, sp_eats_p>>>
            answer(organism1, organism2, opt);
    return answer;
}

/* Walka dwóch organizmów */
template<typename species_t, bool sp1_eats_p, bool sp2_eats_p>
constexpr std::tuple<Organism<species_t, true, sp1_eats_p>,
        Organism<species_t, true, sp2_eats_p>,
        std::optional<Organism<species_t, true, sp1_eats_p>>>
fight(Organism<species_t, true, sp1_eats_p> organism1,
      Organism<species_t, true, sp2_eats_p> organism2) {

    std::optional<Organism<species_t, true, sp1_eats_p>> opt;

    if (organism1.get_vitality() == organism2.get_vitality()) {
        Organism<species_t, true, sp1_eats_p> winner(organism1.get_species(), 0);
        Organism<species_t, true, sp2_eats_p> looser(organism2.get_species(), 0);

        std::tuple<Organism<species_t, true, sp1_eats_p>,
                Organism<species_t, true, sp2_eats_p>,
                std::optional<Organism<species_t, true, sp1_eats_p>>>
                answer(winner, looser, opt);
        return answer;
    } else if (organism1.get_vitality() > organism2.get_vitality()) {
        Organism<species_t, true, sp1_eats_p> winner
                (organism1.get_species(), organism1.get_vitality() + organism2.get_vitality() / 2);

        Organism<species_t, true, sp2_eats_p> looser
                (organism2.get_species(), 0);

        std::tuple<Organism<species_t, true, sp1_eats_p>,
                Organism<species_t, true, sp2_eats_p>,
                std::optional<Organism<species_t, true, sp1_eats_p>>>
                answer(winner, looser, opt);
        return answer;
    } else {
        Organism<species_t, true, sp2_eats_p> winner
                (organism2.get_species(), organism2.get_vitality() + organism1.get_vitality() / 2);
        Organism<species_t, true, sp1_eats_p> looser
                (organism1.get_species(), 0);

        std::tuple<Organism<species_t, true, sp1_eats_p>,
                Organism<species_t, true, sp2_eats_p>,
                std::optional<Organism<species_t, true, sp1_eats_p>>>
                answer(looser, winner, opt);
        return answer;
    }
}

/* Zjedzenie rośliny */
template<typename species_t, bool sp_eats_m>
constexpr std::tuple<Organism<species_t, sp_eats_m, true>,
        Organism<species_t, false, false>,
        std::optional<Organism<species_t, sp_eats_m, true>>>
eatPlant(Organism<species_t, sp_eats_m, true> organism1,
         Organism<species_t, false, false> organism2) {

    std::optional<Organism<species_t, sp_eats_m, true>> opt;
    Organism<species_t, sp_eats_m, true> eating
            (organism1.get_species(), organism1.get_vitality() + organism2.get_vitality());
    Organism<species_t, false, false> eaten
            (organism2.get_species(), 0);

    std::tuple<Organism<species_t, sp_eats_m, true>,
            Organism<species_t, false, false>,
            std::optional<Organism<species_t, sp_eats_m, true>>>
            answer(eating, eaten, opt);
    return answer;
}

/* Pierwszy organizm zjada drugi */
template<typename species_t, bool sp_eats_p>
constexpr std::tuple<Organism<species_t, true, sp_eats_p>,
        Organism<species_t, false, true>,
        std::optional<Organism<species_t, true, sp_eats_p>>>
eatOrganism(Organism<species_t, true, sp_eats_p> organism1,
            Organism<species_t, false, true> organism2) {

    std::optional<Organism<species_t, true, sp_eats_p>> opt;
    Organism<species_t, true, sp_eats_p> eating
            (organism1.get_species(), organism1.get_vitality() + organism2.get_vitality() / 2);
    Organism<species_t, false, true> eaten
            (organism2.get_species(), 0);

    std::tuple<Organism<species_t, true, sp_eats_p>,
            Organism<species_t, false, true>,
            std::optional<Organism<species_t, true, sp_eats_p>>>
            answer(eating, eaten, opt);
    return answer;
}

/* Spotkanie wszystkożercy ze wszystkożercą */
template<typename species_t>
constexpr std::tuple<Organism<species_t, true, true>,
        Organism<species_t, true, true>,
        std::optional<Organism<species_t, true, true>>>
encounter(Organism<species_t, true, true> organism1,
          Organism<species_t, true, true> organism2) {

    if (organism1.is_dead() || organism2.is_dead()) {
        std::optional<Organism<species_t, true, true>> opt;
        std::tuple<Organism<species_t, true, true>,
                Organism<species_t, true, true>,
                std::optional<Organism<species_t, true, true>>>
                answer(organism1, organism2, opt);
        return answer;
    }
    if (organism1.get_species() == organism2.get_species()) {
        return mating(organism1, organism2);
    } else {
        return fight(organism1, organism2);
    }
}

/* Spotkanie mięsożercy z mięsożercą */
template<typename species_t>
constexpr std::tuple<Organism<species_t, true, false>,
        Organism<species_t, true, false>,
        std::optional<Organism<species_t, true, false>>>
encounter(Organism<species_t, true, false> organism1,
          Organism<species_t, true, false> organism2) {

    if (organism1.is_dead() || organism2.is_dead()) {
        std::optional<Organism<species_t, true, false>> opt;
        std::tuple<Organism<species_t, true, false>,
                Organism<species_t, true, false>,
                std::optional<Organism<species_t, true, false>>>
                answer(organism1, organism2, opt);
        return answer;
    }
    if (organism1.get_species() == organism2.get_species()) {
        return mating(organism1, organism2);
    } else {
        return fight(organism1, organism2);
    }
}

/* Spotkanie roślinożercy z roślinożercą */
template<typename species_t>
constexpr std::tuple<Organism<species_t, false, true>,
        Organism<species_t, false, true>,
        std::optional<Organism<species_t, false, true>>>
encounter(Organism<species_t, false, true> organism1,
          Organism<species_t, false, true> organism2) {

    if (organism1.is_dead() || organism2.is_dead()) {
        std::optional<Organism<species_t, false, true>> opt;
        std::tuple<Organism<species_t, false, true>,
                Organism<species_t, false, true>,
                std::optional<Organism<species_t, false, true>>>
                answer(organism1, organism2, opt);
        return answer;
    }
    if (organism1.get_species() == organism2.get_species()) {
        return mating(organism1, organism2);
    } else {
        std::optional<Organism<species_t, false, true>> opt;
        std::tuple<Organism<species_t, false, true>,
                Organism<species_t, false, true>,
                std::optional<Organism<species_t, false, true>>>
                answer(organism1, organism2, opt);
        return answer;
    }
}

/* Spotkanie zwierząt, które jedzą mięsa */
template<typename species_t, bool sp1_eats_p, bool sp2_eats_p>
constexpr std::tuple<Organism<species_t, true, sp1_eats_p>,
        Organism<species_t, true, sp2_eats_p>,
        std::optional<Organism<species_t, true, sp1_eats_p>>>
encounter(Organism<species_t, true, sp1_eats_p> organism1,
          Organism<species_t, true, sp2_eats_p> organism2) {

    if (organism1.is_dead() || organism2.is_dead()) {
        std::optional<Organism<species_t, true, sp1_eats_p>> opt;
        std::tuple<Organism<species_t, true, sp1_eats_p>,
                Organism<species_t, true, sp2_eats_p>,
                std::optional<Organism<species_t, true, sp1_eats_p>>>
                answer(organism1, organism2, opt);
        return answer;
    } else {
        return fight(organism1, organism2);
    }
}

/* Pierwszy organizm - roślinożerca, drugi organizm - wszystkożerca / mięsożerca */
template<typename species_t, bool sp_eats_p>
constexpr std::tuple<Organism<species_t, false, true>,
        Organism<species_t, true, sp_eats_p>,
        std::optional<Organism<species_t, false, true>>>
encounter(Organism<species_t, false, true> organism1,
          Organism<species_t, true, sp_eats_p> organism2) {

    std::tuple<Organism<species_t, true, sp_eats_p>,
            Organism<species_t, false, true>,
            std::optional<Organism<species_t, true, sp_eats_p>>>
            reverse = encounter(organism2, organism1);

    std::optional<Organism<species_t, false, true>> opt;
    std::tuple<Organism<species_t, false, true>,
            Organism<species_t, true, sp_eats_p>,
            std::optional<Organism<species_t, false, true>>>
            answer(get<1>(reverse), get<0>(reverse), opt);
    return answer;
}

/* Pierwszy organizm - mięsożerca / wszystkożerca, drugi organizm - roślinożerca */
template<typename species_t, bool sp_eats_p>
constexpr std::tuple<Organism<species_t, true, sp_eats_p>,
        Organism<species_t, false, true>,
        std::optional<Organism<species_t, true, sp_eats_p>>>
encounter(Organism<species_t, true, sp_eats_p> organism1,
          Organism<species_t, false, true> organism2) {

    if (organism1.is_dead() || organism2.is_dead()) {
        std::optional<Organism<species_t, true, sp_eats_p>> opt;
        std::tuple<Organism<species_t, true, sp_eats_p>,
                Organism<species_t, false, true>,
                std::optional<Organism<species_t, true, sp_eats_p>>>
                answer(organism1, organism2, opt);
        return answer;
    }
    if (organism1.get_vitality() > organism2.get_vitality()) {
        return eatOrganism(organism1, organism2);
    } else {
        std::optional<Organism<species_t, true, sp_eats_p>> opt;
        std::tuple<Organism<species_t, true, sp_eats_p>,
                Organism<species_t, false, true>,
                std::optional<Organism<species_t, true, sp_eats_p>>>
                answer(organism1, organism2, opt);
        return answer;
    }
}

/* Pierwszy organizm - wszystkożerca / roślinożerca, drugi organizm - roślina */
template<typename species_t, bool sp_eats_m>
constexpr std::tuple<Organism<species_t, sp_eats_m, true>,
        Organism<species_t, false, false>,
        std::optional<Organism<species_t, sp_eats_m, true>>>
encounter(Organism<species_t, sp_eats_m, true> organism1,
          Organism<species_t, false, false> organism2) {

    if (organism1.is_dead() || organism2.is_dead()) {
        std::optional<Organism<species_t, sp_eats_m, true>> opt;
        std::tuple<Organism<species_t, sp_eats_m, true>,
                Organism<species_t, false, false>,
                std::optional<Organism<species_t, sp_eats_m, true>>>
                answer(organism1, organism2, opt);
        return answer;
    }
    return eatPlant(organism1, organism2);
}

/* Pierwszy organizm - roślina, drugi organizm - roślinożerca / wszystkożerca */
template<typename species_t, bool sp_eats_m>
constexpr std::tuple<Organism<species_t, false, false>,
        Organism<species_t, sp_eats_m, true>,
        std::optional<Organism<species_t, false, false>>>
encounter(Organism<species_t, false, false> organism1,
          Organism<species_t, sp_eats_m, true> organism2) {

    std::tuple<Organism<species_t, sp_eats_m, true>,
            Organism<species_t, false, false>,
            std::optional<Organism<species_t, sp_eats_m, true>>>
            reverse = encounter(organism2, organism1);

    std::optional<Organism<species_t, false, false>> opt;
    std::tuple<Organism<species_t, false, false>,
            Organism<species_t, sp_eats_m, true>,
            std::optional<Organism<species_t, false, false>>>
            wynik(get<1>(reverse), get<0>(reverse), opt);
    return wynik;
}

/* Pierwszy organizm - roślina, drugi organizm - mięsożerca */
template<typename species_t>
constexpr std::tuple<Organism<species_t, false, false>,
        Organism<species_t, true, false>,
        std::optional<Organism<species_t, false, false>>>
encounter(Organism<species_t, false, false> organism1,
          Organism<species_t, true, false> organism2) {

    std::optional<Organism<species_t, false, false>> opt;
    std::tuple<Organism<species_t, false, false>,
            Organism<species_t, true, false>,
            std::optional<Organism<species_t, false, false>>>
            answer(organism1, organism2, opt);
    return answer;
}

/* Pierwszy organizm - mięsożerca, drugi organizm - roślina */
template<typename species_t>
constexpr std::tuple<Organism<species_t, true, false>,
        Organism<species_t, false, false>,
        std::optional<Organism<species_t, true, false>>>
encounter(Organism<species_t, true, false> organism1,
          Organism<species_t, false, false> organism2) {

    std::optional<Organism<species_t, true, false>> opt;
    std::tuple<Organism<species_t, true, false>,
            Organism<species_t, false, false>,
            std::optional<Organism<species_t, true, false>>>
            answer(organism1, organism2, opt);
    return answer;
}

/* Funkcja pomocnicza do serii spotkań organizmów  */
template<typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p, typename ... Args>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
encounter_helper(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
                 Organism<species_t, sp2_eats_m, sp2_eats_p> organism2,
                 Args ... args) {

    if constexpr (sizeof ...(args) == 0) {
        return get<0>(encounter(organism1, organism2));
    } else {
        return encounter_helper(get<0>(encounter(organism1, organism2)), args...);
    }
}

/* Seria spotkań organizmów */
template<typename species_t, bool sp1_eats_m, bool sp1_eats_p, typename ... Args>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
encounter_series(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1, Args ... args) {

    if constexpr (sizeof ...(args) == 0) {
        return organism1;
    } else {
        return encounter_helper(organism1, args...);
    }
}


template<typename species_t>
using Carnivore = Organism<species_t, true, false>;

template<typename species_t>
using Omnivore = Organism<species_t, true, true>;

template<typename species_t>
using Herbivore = Organism<species_t, false, true>;

template<typename species_t>
using Plant = Organism<species_t, false, false>;

#endif // ORGANISM_H