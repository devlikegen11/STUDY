#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <vector>
#include <mariadb/conncpp.hpp>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include <curl/curl.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

using json = nlohmann::json;

void *handle_clnt(void *arg);
void send_msg(const char *msg, int len, int sender_sock, std::string id);
void error_handling(const char *msg);
std::string url_encode(const std::string &value);
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
std::string html_decode(const std::string &str);
std::string remove_tags(const std::string &str, const std::vector<std::string> &tags);
std::string replace_all(const std::string &str, const std::string &from, const std::string &to);
std::string extract_tag(const std::string &xml, const std::string &tag);
std::vector<std::string> extract_tag_content(const std::string &xml, const std::string &tag);
std::pair<std::string, std::string> country_capital(const std::string &basic_content);
bool read_image(const char *filepath, char **buf, int *len);

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
int user_socks[MAX_CLNT];
int cs_socks[MAX_CLNT];
bool cs_available[MAX_CLNT];
int user_to_cs[MAX_CLNT];
int cs_to_user[MAX_CLNT];

int user_infonum;
std::string user_id;
pthread_mutex_t mutx;

class DB
{
protected:
    sql::Connection *conn;

public:
    sql::PreparedStatement *prepareStatement(const std::string &query) // ? 쿼리문 삽입
    {
        sql::PreparedStatement *stmt(conn->prepareStatement(query));
        return stmt;
    }

    sql::Statement *createStatement() // 쿼리문 즉시 실행
    {
        sql::Statement *stm(conn->createStatement());
        return stm;
    }

    void connect()
    {
        try
        {
            sql::Driver *driver = sql::mariadb::get_driver_instance();
            sql::SQLString url("jdbc:mariadb://10.10.21.115:3306/STUDYTEST");
            sql::Properties properties({{"user", "SUTEST"}, {"password", "1"}});
            conn = driver->connect(url, properties);
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error Connecting to MariaDB Platform: " << e.what() << std::endl;
        }
    }

    bool attemptLogin(const std::string &ID, const std::string &PW) // 유저 로그인 bool타입으로 검사
    {
        try
        {
            std::cout << "유저 접속어탬프" << std::endl;
            std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
            sql::ResultSet *res = stmnt->executeQuery("SELECT * FROM USER");
            while (res->next())
            {
                if (res->getString(2) == ID && res->getString(3) == PW)
                {
                    return true;
                }
            }
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error during login attempt: " << e.what() << std::endl;
        }
        return false;
    }

    bool attemptCS(const std::string &ID, const std::string &PW) // 상담원 로그인 bool타입으로 검사
    {
        try
        {
            std::cout << "상담원 접속어탬프" << std::endl;
            std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
            sql::ResultSet *res2 = stmnt->executeQuery("SELECT * FROM CS");
            while (res2->next())
            {
                if (res2->getString(2) == ID && res2->getString(3) == PW)
                {
                    return true;
                }
            }
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error during login attempt: " << e.what() << std::endl;
        }
        return false;
    }

    json userinfo(const std::string &userid) // 사용자 정보 JSON[JSON]형식으로 만들어서 리턴
    {
        json user_info;
        try
        {

            std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("SELECT * FROM USER WHERE USER_ID = ?;"));
            stmnt->setString(1, userid);
            std::unique_ptr<sql::ResultSet> res3(stmnt->executeQuery());

            while (res3->next())
            {
                user_info["USER_NO"] = res3->getInt("USER_NO");
                user_info["USER_ID"] = res3->getString("USER_ID");
                user_info["USER_PASSWORD"] = res3->getString("USER_PASSWORD");
                user_info["USER_NAME"] = res3->getString("USER_NAME");
                user_info["USER_PHONE"] = res3->getString("USER_PHONE");
                user_info["USER_STATE"] = res3->getString("USER_STATE");
            }
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error fetching user info: " << e.what() << std::endl;
        }
        return user_info;
    }

    void InStudy(std::string country, std::string capital)
    {
        try
        {
            std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("INSERT INTO STUDY (ST_CON, ST_CITY) VALUES (?, ?)"));
            stmnt->setString(1, country);
            stmnt->setString(2, capital);
            stmnt->executeQuery();
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error inserting new task: " << e.what() << std::endl;
        }
    }

    json setstudy()
    {
        json question;
        std::vector<json> study_q;
        try
        {
            std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("SELECT ST_CON, ST_CITY FROM STUDY;"));
            std::unique_ptr<sql::ResultSet> res5(stmnt->executeQuery());

            while (res5->next())
            {
                json st_cord;
                st_cord["ST_CON_K"] = res5->getString("ST_CON");
                st_cord["ST_CITY_K"] = res5->getString("ST_CITY");
                study_q.push_back(st_cord);
            }
            question["STUDY_GO"] = study_q;
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error fetching user info: " << e.what() << std::endl;
        }
        return question;
    }

    json settest()
    {
        json testset;
        std::vector<json> test_q;
        try
        {
            std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("SELECT ST_CON, ST_CITY FROM STUDY order by rand();"));
            std::unique_ptr<sql::ResultSet> res6(stmnt->executeQuery());

            while (res6->next())
            {
                json test_cord;
                test_cord["ST_CON_K"] = res6->getString("ST_CON");
                test_cord["ST_CITY_K"] = res6->getString("ST_CITY");
                test_q.emplace_back(test_cord);
            }
            testset["TEST_GO"] = test_q;
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error fetching user info: " << e.what() << std::endl;
        }
        return testset;
    }

    void INSCORE(std::string score, std::string user_id)
    {
        try
        {
            std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("INSERT INTO SCORE (SCORE, SUSER_ID) VALUES (?, ?)"));
            stmnt->setString(1, score);
            stmnt->setString(2, user_id);
            stmnt->executeQuery();
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error inserting new task: " << e.what() << std::endl;
        }
    }

    json SHOWSCORE(const std::string &userid)
    {
        json user_search;
        std::vector<json> score_record;
        try
        {
            std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("SELECT USER.USER_ID, USER.USER_STATE, SCORE.SCORE, SCORE.SUSER_ID FROM SCORE LEFT OUTER JOIN USER ON SCORE.SUSER_ID = USER.USER_ID WHERE USER.USER_ID = ?;"));
            stmnt->setString(1, userid);
            std::unique_ptr<sql::ResultSet> res7(stmnt->executeQuery());

            while (res7->next())
            {
                json score_cord;
                score_cord["SCORE_RECORD"] = res7->getString("SCORE");
                score_cord["USER_ID"] = res7->getString("SUSER_ID");
                score_record.push_back(score_cord);
            }
            user_search["SCORE_GO"] = score_record;
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error fetching user info: " << e.what() << std::endl;
        }
        return user_search;
    }

    void USERJOIN(std::string ID, std::string PW, std::string NAME, int phone, std::string STATE)
    {
        try
        {
            std::cout << "회원가입 진행" << std::endl;
            std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("INSERT INTO USER(USER_ID, USER_PASSWORD, USER_NAME, USER_PHONE, USER_STATE) VALUES (?, ?, ?, ?, ?);"));
            stmnt->setString(1, ID);
            stmnt->setString(2, PW);
            stmnt->setString(3, NAME);
            stmnt->setInt(4, phone);
            stmnt->setString(5, STATE);
            stmnt->executeQuery();
        }
        catch (const std::exception &e)
        {
            std::cerr << "회원가입 에러: " << e.what() << std::endl;
        }
    }

    ~DB() { delete conn; }
};

void *handle_clnt(void *arg)
{
    int clnt_sock = *((int *)arg);
    int str_len = 0;
    char msg[BUF_SIZE];
    bool is_user = false;
    bool is_consultant = false;
    int mapped_cs_sock = -1;

    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
    {
        json received_json = json::parse(std::string(msg, str_len));
        std::string client_type = received_json["type"];
        DB db;
        db.connect();

        char *buf = nullptr;
        int len = 0;

        if (client_type == "user")
        {
            user_id = received_json["id"];
            std::string pw = received_json["pw"];
            bool login_success = db.attemptLogin(user_id, pw);

            json response;
            response["login_success"] = login_success;
            std::string response_str = response.dump();
            write(clnt_sock, response_str.c_str(), response_str.length());

            if (login_success) // 유저 채팅 로직
            {
                is_user = true;

                pthread_mutex_lock(&mutx);
                for (int i = 0; i < MAX_CLNT; i++)
                {
                    if (cs_available[i])
                    {
                        mapped_cs_sock = cs_socks[i];
                        user_to_cs[clnt_sock] = cs_socks[i];
                        cs_to_user[cs_socks[i]] = clnt_sock;
                        cs_available[i] = false;
                        break;
                    }
                }
                pthread_mutex_unlock(&mutx);

                while (true)
                {
                    str_len = read(clnt_sock, msg, sizeof(msg) - 1);
                    if (str_len <= 0)
                    {
                        break; // 클라이언트 연결이 끊어지면 루프 종료
                    }

                    msg[str_len] = '\0';
                    std::string received_msg(msg);

                    if (received_msg == "CHAT")
                    {
                        std::cout << "유저 채팅 진입" << std::endl;
                        while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
                        {
                            msg[str_len] = '\0';
                            std::cout << msg << std::endl;
                            send_msg(msg, str_len, clnt_sock, user_id);
                        }
                    }
                    else if (received_msg == "STUDY")
                    {
                        json send_study = db.setstudy();
                        std::string study_str = send_study.dump();
                        write(clnt_sock, study_str.c_str(), study_str.length());
                        sleep(1);

                        if (read_image("/home/lms/Downloads/world", &buf, &len))
                        {
                            // 이미지 데이터 크기 전송
                            int image_size = htonl(len); // 크기를 네트워크 바이트 순서로 변환
                            if (send(clnt_sock, &image_size, sizeof(image_size), 0) == -1)
                            {
                                perror("이미지 크기 전송 실패");
                            }
                            std::cout << "이미지 크기" << image_size << std::endl;

                            // 이미지 데이터 전송
                            if (send(clnt_sock, buf, len, 0) == -1)
                            {
                                perror("이미지 전송 실패");
                            }
                            else
                            {
                                std::cout << "이미지 전송 완료" << std::endl;
                                std::cout << "이미지 데이터" << buf << std::endl;
                            }
                            free(buf);
                        }
                    }
                    else if (received_msg == "TEST")
                    {
                        std::cout << "시험" << std::endl;
                        json send_test = db.settest();
                        std::string test_str = send_test.dump();
                        write(clnt_sock, test_str.c_str(), test_str.length());

                        memset(msg, 0, BUF_SIZE);
                        str_len = read(clnt_sock, msg, sizeof(msg) - 1);
                        received_msg = std::string(msg);
                        std::cout << received_msg << std::endl;
                        db.INSCORE(received_msg, user_id);
                    }
                }
            }
        }
        else if (client_type == "consultant")
        {
            std::string id = received_json["id"];
            std::string pw = received_json["pw"];
            bool login_success = db.attemptCS(id, pw);
            json response;
            response["login_success"] = login_success;
            std::string response_str = response.dump();
            write(clnt_sock, response_str.c_str(), response_str.length());

            if (login_success)
            {
                is_consultant = true;

                pthread_mutex_lock(&mutx);
                for (int i = 0; i < MAX_CLNT; i++)
                {
                    if (cs_socks[i] == 0)
                    {
                        cs_socks[i] = clnt_sock;
                        cs_available[i] = true;
                        break;
                    }
                }
                pthread_mutex_unlock(&mutx);

                while (true)
                {
                    str_len = read(clnt_sock, msg, sizeof(msg) - 1);
                    if (str_len <= 0)
                    {
                        break; // 클라이언트 연결이 끊어지면 루프 종료
                    }

                    msg[str_len] = '\0';
                    std::string received_msg(msg);

                    if (received_msg == "CHAT")
                    {
                        std::cout << "상담원 채팅 진입" << std::endl;
                        while (true)
                        {
                            memset(msg, 0, BUF_SIZE);
                            str_len = read(clnt_sock, msg, sizeof(msg) - 1);
                            std::cout << msg << std::endl;
                            if (str_len <= 0)
                            {
                                break; // 클라이언트 연결이 끊어지면 채팅 모드 종료
                            }

                            msg[str_len] = '\0';
                            received_msg = std::string(msg);
                            send_msg(msg, str_len, clnt_sock, id);
                        }
                    }
                    else if (received_msg == "SCORE_CHECK")
                    {
                        memset(msg, 0, BUF_SIZE);
                        str_len = read(clnt_sock, msg, sizeof(msg) - 1);
                        msg[strlen(msg)] = 0;
                        received_msg = std::string(msg);
                        json user_info = db.userinfo(received_msg);
                        json score_history = db.SHOWSCORE(received_msg);

                        json combined;
                        combined["user_info"] = user_info;
                        combined["SCORE"] = score_history;

                        std::string response_str = combined.dump();
                        write(clnt_sock, response_str.c_str(), response_str.length());
                    }
                }
            }
        }
        else if (client_type == "JOIN")
        {
            std::cout << "회원가입" << std::endl;
            memset(msg, 0, BUF_SIZE);
            str_len = read(clnt_sock, msg, sizeof(msg) - 1);
            json join_plz = json::parse(std::string(msg, str_len));
            std::string ID = join_plz["id"];
            std::string PW = join_plz["pw"];
            std::string NAME = join_plz["name"];
            int phone = join_plz["phone"];
            std::string STATE = join_plz["state"];

            db.USERJOIN(ID, PW, NAME, phone, STATE);
        }
    }

    pthread_mutex_lock(&mutx);
    for (int i = 0; i < clnt_cnt; i++)
    {
        if (clnt_sock == clnt_socks[i])
        {
            while (i++ < clnt_cnt - 1)
            {
                clnt_socks[i] = clnt_socks[i + 1];
            }
            break;
        }
    }
    clnt_cnt--;

    if (is_user)
    {
        int cs_sock = user_to_cs[clnt_sock];
        for (int i = 0; i < MAX_CLNT; i++)
        {
            if (cs_socks[i] == cs_sock)
            {
                cs_available[i] = true;
                break;
            }
        }
        user_to_cs[clnt_sock] = 0;
        cs_to_user[cs_sock] = 0;
    }

    if (is_consultant)
    {
        for (int i = 0; i < MAX_CLNT; i++)
        {
            if (cs_socks[i] == clnt_sock)
            {
                cs_socks[i] = 0;
                cs_available[i] = false;
                if (cs_to_user[clnt_sock] != 0)
                {
                    user_to_cs[cs_to_user[clnt_sock]] = 0;
                    cs_to_user[clnt_sock] = 0;
                }
                break;
            }
        }
    }

    pthread_mutex_unlock(&mutx);
    close(clnt_sock);

    return NULL;
}

void send_msg(const char *msg, int len, int sender_sock, std::string id)
{
    DB db;
    db.connect();
    pthread_mutex_lock(&mutx);
    int recipient_sock = -1;

    if (user_to_cs[sender_sock] != 0)
    {
        recipient_sock = user_to_cs[sender_sock];
    }
    else if (cs_to_user[sender_sock] != 0)
    {
        recipient_sock = cs_to_user[sender_sock];
    }

    if (recipient_sock != -1)
    {
        write(recipient_sock, msg, len);
    }

    pthread_mutex_unlock(&mutx);
}

void error_handling(const char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    DB db;
    db.connect();
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    std::string serviceKey = "NUxs56M+nB/ByvDH5zA3jj70T9/Hkz+BsHj6+WRG3g5KMz6EbJ2g2Q2xLJ3OLHCT9qmXzpXbIwd/LXrFWFBQgw==";
    std::string url = "http://apis.data.go.kr/1262000/CountryBasicService/getCountryBasicList";
    url += "?ServiceKey=" + url_encode(serviceKey);
    url += "&numOfRows=10";
    url += "&pageNo=1";
    url += "&isoCode1=ALB";
    url += "&isoCode2=DZA";
    url += "&isoCode3=PRT";
    url += "&isoCode4=VNM";
    url += "&isoCode5=LBN";
    url += "&isoCode6=SUR";
    url += "&isoCode7=COL";
    url += "&isoCode8=BRA";
    url += "&isoCode9=GRC";
    url += "&isoCode10=HUN";
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else
        {
            std::vector<std::string> items = extract_tag_content(readBuffer, "item");
            for (const std::string &item : items)
            {
                std::string basic_content = extract_tag(item, "basic");
                basic_content = html_decode(basic_content);
                std::vector<std::string> tags_to_remove = {"div", "br", "p"};
                basic_content = remove_tags(basic_content, tags_to_remove);
                basic_content = replace_all(basic_content, "\n", " ");
                auto [country, capital] = country_capital(basic_content);
                db.InStudy(country, capital);
            }
        }
        curl_easy_cleanup(curl);
    }

    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    pthread_t t_id;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);
    memset(user_to_cs, 0, sizeof(user_to_cs));
    memset(cs_to_user, 0, sizeof(cs_to_user));

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
    {
        error_handling("bind() error");
    }
    if (listen(serv_sock, 5) == -1)
    {
        error_handling("listen() error");
    }

    while (1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);

        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutx);

        pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_sock);
        pthread_detach(t_id);
        printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
    }

    close(serv_sock);
    return 0;
}

std::string url_encode(const std::string &value)
{
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : value)
    {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            escaped << c;
            continue;
        }
        escaped << '%' << std::setw(2) << int((unsigned char)c);
    }

    return escaped.str();
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

std::string html_decode(const std::string &str)
{
    std::unordered_map<std::string, char> html_entities = {
        {"&lt;", '<'}, {"&gt;", '>'}, {"&amp;", '&'}, {"&quot;", '"'}, {"&apos;", '\''}, {"&#xD;", '\n'}};
    std::string result;
    size_t pos = 0;
    while (pos < str.size())
    {
        if (str[pos] == '&')
        {
            size_t semi_pos = str.find(';', pos);
            if (semi_pos != std::string::npos)
            {
                std::string entity = str.substr(pos, semi_pos - pos + 1);
                if (html_entities.find(entity) != html_entities.end())
                {
                    result += html_entities[entity];
                    pos = semi_pos + 1;
                    continue;
                }
            }
        }
        result += str[pos++];
    }
    return result;
}

std::string remove_tags(const std::string &str, const std::vector<std::string> &tags)
{
    std::string result = str;
    for (const auto &tag : tags)
    {
        std::string open_tag = "<" + tag;
        std::string close_tag = "</" + tag + ">";
        size_t pos = 0;
        while ((pos = result.find(open_tag, pos)) != std::string::npos)
        {
            size_t end_pos = result.find('>', pos);
            if (end_pos != std::string::npos)
            {
                result.erase(pos, end_pos - pos + 1);
            }
            else
            {
                break;
            }
        }
        while ((pos = result.find(close_tag)) != std::string::npos)
        {
            result.erase(pos, close_tag.length());
        }
    }
    return result;
}

std::string replace_all(const std::string &str, const std::string &from, const std::string &to)
{
    if (from.empty())
        return str; // from 문자열이 빈 문자열인 경우 처리
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos)
    {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

std::string extract_tag(const std::string &xml, const std::string &tag)
{
    std::string open_tag = "<" + tag + ">";
    std::string close_tag = "</" + tag + ">";
    size_t start = xml.find(open_tag);
    if (start == std::string::npos)
        return "";
    start += open_tag.length();
    size_t end = xml.find(close_tag, start);
    if (end == std::string::npos)
        return "";
    return xml.substr(start, end - start);
}

std::vector<std::string> extract_tag_content(const std::string &xml, const std::string &tag)
{
    std::vector<std::string> contents;
    std::string open_tag = "<" + tag + ">";
    std::string close_tag = "</" + tag + ">";
    size_t start = 0;
    while ((start = xml.find(open_tag, start)) != std::string::npos)
    {
        start += open_tag.length();
        size_t end = xml.find(close_tag, start);
        if (end == std::string::npos)
            break;
        contents.push_back(xml.substr(start, end - start));
        start = end + close_tag.length();
    }
    return contents;
}

std::pair<std::string, std::string> country_capital(const std::string &basic_content)
{
    DB db;
    std::string country, capital;
    size_t country_pos = basic_content.find("ㅇ 국명 : ");
    if (country_pos != std::string::npos)
    {
        size_t end_pos = basic_content.find("ㅇ", country_pos + 1);
        country = basic_content.substr(country_pos + 13, end_pos - country_pos - 13);
        size_t capital_pos = basic_content.find("ㅇ 수도 : ", end_pos);
        if (capital_pos != std::string::npos)
        {
            size_t end_pos2 = basic_content.find("ㅇ", capital_pos + 1);
            capital = basic_content.substr(capital_pos + 13, end_pos2 - capital_pos - 13);
        }
    }
    db.InStudy(country, capital);
    return {country, capital};
}

bool read_image(const char *filepath, char **buf, int *len)
{
    FILE *fp = fopen(filepath, "rb");
    if (!fp)
    {
        perror("파일 열기 실패");
        return false;
    }

    fseek(fp, 0, SEEK_END);
    *len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    *buf = (char *)malloc(*len);
    if (!*buf)
    {
        perror("메모리 할당 실패");
        fclose(fp);
        return false;
    }

    size_t bytesRead = fread(*buf, 1, *len, fp);
    if (bytesRead != *len)
    {
        perror("파일 읽기 실패");
        free(*buf);
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}