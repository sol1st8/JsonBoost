#include <boost/json.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace json = boost::json;

struct CastingRole {
    std::string star;
    std::string name;
};

struct Movie {
    int id;
    std::string title;
    int year;
    int length;
    std::vector<CastingRole> cast;
    std::vector<std::string> directors;
    std::vector<std::string> writers;
};

using movie_list = std::vector<Movie>;

void tag_invoke(json::value_from_tag, json::value& jv, const CastingRole& casting_role) {
    jv = {
        {"star", casting_role.star},
        {"name", casting_role.name}
    };
}

void tag_invoke(json::value_from_tag, json::value& jv, const Movie& movie) {
    jv = {
        {"id", movie.id},
        {"title", movie.title},
        {"year", movie.year},
        {"length", movie.length},
        {"cast", movie.cast},
        {"directors", movie.directors},
        {"writers", movie.writers}
    };
}

CastingRole tag_invoke(json::value_to_tag<CastingRole>&, const json::value casting_role) {
    CastingRole cast;
    cast.star = casting_role.at("star").as_string();
    cast.name = casting_role.at("name").as_string();
    return cast;
}

Movie tag_invoke(json::value_to_tag<Movie>&, const json::value movie) {
    Movie m;
    m.id = movie.at("id").as_int64();
    m.title = movie.at("title").as_string();
    m.year = movie.at("year").as_int64();
    m.length = movie.at("length").as_int64();

    for (const auto& cast_item : movie.at("cast").as_array()) {
        m.cast.push_back(json::value_to<CastingRole>(cast_item));
    }

    auto directors = json::value_to<std::vector<std::string>>(movie.at("directors"));
    for (auto& director : directors) {
        m.directors.push_back(director);
    }

    auto writers = json::value_to<std::vector<std::string>>(movie.at("writers"));
    for (auto& writer : writers) {
        m.writers.push_back(writer);
    }

    return m;
}

int main() {
    std::ifstream ifile("/home/enxsz/PetProjects/JsonBoost/movies.json");
    std::string json_str;

    if (ifile.is_open()) {
        json_str = std::string(std::istreambuf_iterator<char>(ifile), std::istreambuf_iterator<char>());
    }

    movie_list movies_l;

    auto jdata = json::parse(json_str);
    auto movies = json::value_to<movie_list>(jdata.at("movies"));
    for (auto& movie : movies) {
        movies_l.push_back(movie);
    }

    assert(movies_l.size() == 2);
    assert(movies_l[0].title == "The Matrix");
    assert(movies_l[1].title == "Forrest Gump");

    movie_list m
    {
       {
          11001,
          "The Matrix",
          1999,
          196,
          {
             {"Keanu Reeves", "Neo"},
             {"Laurence Fishburne", "Morpheus"},
             {"Carrie-Anne Moss", "Trinity"},
             {"Hugo Weaving", "Agent Smith"}
          },
          {"Lana Wachowski", "Lilly Wachowski"},
          {"Lana Wachowski", "Lilly Wachowski"},
       },
       {
          9871,
          "Forrest Gump",
          1994,
          202,
          {
             {"Tom Hanks", "Forrest Gump"},
             {"Sally Field", "Mrs. Gump"},
             {"Robin Wright","Jenny Curran"},
             {"Mykelti Williamson","Bubba Blue"}
          },
          {"Robert Zemeckis"},
          {"Winston Groom", "Eric Roth"},
       }
    };

    json::value jmovies = json::value_from(m);
    json::object data = {{"movies", jmovies}};

    std::ofstream ofile("../m.json");
    if (ofile.is_open()) {
        ofile << json::serialize(data);
    }
}
