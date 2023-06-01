#include <sys/select.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <queue>
#include <time.h>
#include <cstdlib>
#define MAXLINE 10000
#define SA struct sockaddr
#define LISTENQ 1024
using namespace std;

int animalNum = 889;
string animals[] = { "aardvark", "aardwolf", "abalone", "acouchi", "addax", "adder", "adouri", "agama", "agouti", "aidi", "airedale", "akitainu", "albino", "alleycat", "alpaca", "ammonite", "amoeba", "amphiuma", "anaconda", "anchovy", "anemone", "angora", "anhinga", "ankole", "annelid", "annelida", "anole", "ant", "antbear", "anteater", "antelope", "antlion", "anura", "aoudad", "ape", "aphid", "aracari", "arachnid", "arawana", "argali", "arkshell", "armyant", "armyworm", "arrowana", "aruanas", "asp", "ass", "astarte", "auk", "auklet", "aurochs", "avians", "avocet", "axisdeer", "axolotl", "ayeaye", "aztecant", "babirusa", "baboon", "bactrian", "badger", "bagworm", "baiji", "banteng", "barb", "barbet", "barnacle", "barnowl", "basenji", "basil", "basilisk", "bass", "bat", "bats", "beagle", "bear", "beaver", "bedbug", "bee", "beetle", "bellfrog", "betafish", "bettong", "bighorn", "bilby", "bird", "bison", "bittern", "blackfly", "blacklab", "blesbok", "blobfish", "blowfish", "bluebird", "bluefish", "bluegill", "bluejay", "bluet", "boa", "boar", "bobcat", "bobolink", "bobwhite", "bongo", "bonobo", "booby", "borer", "borzoi", "boto", "boubou", "boutu", "bovine", "brant", "bream", "bronco", "buck", "budgie", "bufeo", "buffalo", "bug", "bull", "bullfrog", "bunny", "bunting", "burro", "bushbaby", "bustard", "buzzard", "caiman", "calf", "camel", "canary", "canine", "capybara", "caracal", "cardinal", "caribou", "carp", "cat", "catbird", "catfish", "cats", "cattle", "caudata", "cavy", "cero", "chafer", "chamois", "cheetah", "chick", "chicken", "chipmunk", "chital", "cicada", "cirriped", "civet", "clam", "clumber", "coati", "cob", "cobra", "cock", "cockatoo", "cod", "collie", "colt", "comet", "conch", "condor", "coney", "conure", "cony", "coot", "cooter", "copepod", "coqui", "coral", "cougar", "cow", "cowbird", "cowrie", "coyote", "coypu", "crab", "crane", "cranefly", "crayfish", "creature", "cricket", "crow", "cub", "cuckoo", "cur", "curassow", "curlew", "cuscus", "cutworm", "cygnet", "dairycow", "dartfrog", "dassie", "deer", "degu", "degus", "dikdik", "dikkops", "dingo", "dinosaur", "dipper", "discus", "dodo", "dodobird", "doe", "dog", "dogfish", "dolphin", "donkey", "dorado", "dore", "dorking", "dormouse", "dotterel", "dove", "dragon", "drake", "drever", "drongo", "duck", "duckling", "dugong", "duiker", "dunlin", "dunnart", "eagle", "earwig", "echidna", "eel", "eeve", "eft", "egg", "egret", "eider", "eland", "elephant", "elk", "elkhound", "elver", "emu", "equine", "erin", "ermine", "erne", "esok", "ewe", "eyas", "eyra", "fairyfly", "falcon", "fantail", "fanworms", "fawn", "feline", "ferret", "finch", "finwhale", "fireant", "firefly", "fish", "flamingo", "flatfish", "flea", "flee", "flicker", "flies", "flounder", "fluke", "fly", "foal", "fossa", "fowl", "fox", "foxhound", "frog", "fruitbat", "fruitfly", "fugu", "fulmar", "furseal", "gadwall", "galago", "galah", "gander", "gannet", "gar", "garpike", "gaur", "gavial", "gazelle", "gecko", "geese", "gelada", "gelding", "gemsbok", "gemsbuck", "genet", "gerbil", "gerenuk", "gharial", "gibbon", "giraffe", "glowworm", "gnat", "gnu", "goa", "goat", "godwit", "goitered", "goldfish", "gonolek", "goose", "gopher", "goral", "gorilla", "goshawk", "gosling", "gourami", "grackle", "grayfox", "grayling", "graywolf", "grebe", "grison", "grosbeak", "grouper", "grouse", "grub", "grunion", "guanaco", "gull", "guppy", "hackee", "haddock", "hagfish", "hake", "halcyon", "halibut", "halicore", "hamster", "hapuka", "hapuku", "hare", "harpseal", "harrier", "hart", "hawk", "hedgehog", "heifer", "hen", "herald", "heron", "herring", "hind", "hoatzin", "hog", "hogget", "hoiho", "hoki", "honeybee", "hoopoe", "hornbill", "hornet", "horse", "horsefly", "hound", "housefly", "hoverfly", "huemul", "huia", "human", "husky", "hydra", "hyena", "hyrax", "ibex", "ibis", "ibisbill", "icefish", "ichidna", "iggypops", "iguana", "imago", "impala", "incatern", "inchworm", "indri", "inganue", "insect", "isopod", "jabiru", "jackal", "jaeger", "jaguar", "javalina", "jay", "jenny", "jerboa", "joey", "johndory", "junco", "junebug", "kagu", "kakapo", "kangaroo", "karakul", "katydid", "kawala", "kestrel", "kid", "killdeer", "kingbird", "kinglet", "kinkajou", "kiskadee", "kite", "kitfox", "kitten", "kitty", "kiwi", "koala", "kob", "koi", "koodoo", "kouprey", "krill", "kudu", "lacewing", "ladybird", "ladybug", "lamb", "lamprey", "langur", "lark", "larva", "lcont", "leafbird", "leafwing", "leech", "lemming", "lemur", "leopard", "leveret", "lice", "liger", "limpet", "limpkin", "ling", "lion", "lionfish", "lizard", "llama", "lobo", "lobster", "locust", "longhorn", "longspur", "loon", "lorikeet", "loris", "louse", "lovebird", "lowchen", "lunamoth", "lungfish", "lynx", "lynx", "macaque", "macaw", "macropod", "maggot", "magpie", "malamute", "mallard", "mamba", "mammal", "mammoth", "manatee", "mandrill", "mangabey", "manta", "mantaray", "mantid", "mantis", "manxcat", "mara", "marabou", "mare", "marlin", "marmoset", "marmot", "marten", "martin", "mastiff", "mastodon", "mayfly", "mealworm", "meerkat", "merlin", "mice", "midge", "mink", "minnow", "mite", "moa", "mole", "mollies", "mollusk", "molly", "monarch", "mongoose", "mongrel", "monkey", "moorhen", "moose", "moray", "morayeel", "morpho", "mosasaur", "mosquito", "moth", "motmot", "mouflon", "mouse", "mudpuppy", "mule", "mullet", "muntjac", "murrelet", "muskox", "muskrat", "mussel", "mustang", "mutt", "myna", "mynah", "myotis��", "nabarlek", "nag", "naga", "nagapies", "nandine", "nandoo", "nandu", "narwhal", "narwhale", "nauplius", "nautilus", "nematode", "nene", "nerka", "newt", "nightjar", "nilgai", "noctilio", "noctule", "noddy", "numbat", "nuthatch", "nutria", "nyala", "nymph", "ocelot", "octopus", "okapi", "olingo", "olm", "opossum", "orca", "oriole", "oryx", "osprey", "ostracod", "ostrich", "otter", "ovenbird", "owl", "oxen", "oxpecker", "oyster", "paca��", "panda", "pangolin", "panther", "papillon", "parakeet", "parrot", "peacock", "peafowl", "peccary", "pelican", "penguin", "perch", "pewee", "pheasant", "phoebe", "phoenix", "pig", "pigeon", "piglet", "pika", "pike", "pilchard", "pinniped", "pintail", "pipit", "piranha", "pitbull", "plankton", "platypus", "plover", "polecat", "polliwog", "polyp", "pony", "pooch", "poodle", "porpoise", "possum", "prawn", "primate", "puffer", "puffin", "pug", "pullet", "puma", "pupa", "pupfish", "puppy", "pussycat", "pygmy", "python", "quagga", "quahog", "quail", "queenant", "queenbee", "quelea", "quetzal", "quokka", "quoll", "rabbit", "raccoon", "racer", "ragfish", "rail", "ram", "raptors", "rasbora", "rat", "ratfish", "rattail", "raven", "ray", "redhead", "redpoll", "redstart", "reindeer", "reptile", "reynard", "rhea", "rhino", "ringworm", "roach", "roan", "robin", "rockrat", "rodent", "roebuck", "roller", "rook", "rooster", "sable", "saiga", "salmon", "sambar", "sardine", "sawfish", "scallop", "scarab", "scaup", "scorpion", "scoter", "screamer", "seabird", "seagull", "seahog", "seahorse", "seal", "sealion", "seaslug", "seriema", "serpent", "serval", "shark", "sheep", "shelduck", "shibainu", "shihtzu", "shoveler", "shrew", "shrike", "shrimp", "siamang", "sifaka", "silkworm", "siskin", "skimmer", "skink", "skipper", "skua", "skunk", "skylark", "sloth", "slug", "smelts", "smew", "snail", "snake", "snipe", "snowdog", "snowyowl", "songbird", "sora", "sow", "sparrow", "sphinx", "spider", "sponge", "squab", "squamata", "squeaker", "squid", "squirrel", "stag", "stallion", "starfish", "starling", "steed", "steer", "stilt", "stingray", "stinkbug", "stinkpot", "stoat", "stonefly", "stork", "stud", "sturgeon", "sunbear", "sunfish", "swallow", "swan", "swift", "tadpole", "tahr", "takin", "tamarin", "tanager", "tapaculo", "tapeworm", "tapir", "tarpan", "tarsier", "taruca", "tattler", "tayra", "teal", "tegus", "teledu", "tench", "tenrec", "termite", "tern", "terrapin", "terrier", "thrasher", "thrip", "thrush", "tick", "tiger", "tilefish", "tinamou", "titi", "titmouse", "toad", "toadfish", "tomtit", "topi", "tortoise", "toucan", "towhee", "tragopan", "trex", "trogon", "trout", "tuatara", "tuna", "turaco", "turkey", "turtle", "uakari", "umbrette", "unau", "ungulate", "unicorn", "upupa", "urchin", "urial", "urson", "urubu", "urus", "urutu", "urva", "vaquita", "veery", "verdin", "vervet", "vicuna", "viper", "vireo", "vixen", "vole", "volvox", "vulture", "wallaby", "wallaroo", "walleye", "walrus", "warbler", "warthog", "wasp", "waterbug", "watussi", "waxwing", "weasel", "weevil", "whale", "whapuku", "whelp", "whimbrel", "whippet", "whiteeye", "whooper", "widgeon", "wildcat", "willet", "wireworm", "wisent", "wolf", "wombat", "woodcock", "worm", "wrasse", "wren", "wryneck", "wyvern", "xantus", "xeme", "xenops", "xenopus", "xenurine", "xerus", "xiphias", "xoni", "xrayfish", "yaffle", "yak", "yapok", "yardant", "yearling", "yeti", "ynambu", "yucker", "zander", "zebra", "zebu", "zenaida", "zeren", "zopilote", "zorilla" };
queue<string> leftUsers;

string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}
bool isName(char* buf) {
        char name[] = "/name ";
        for(int i = 0; i <= 5; i++) {
                if(buf[i] != name[i]) {
                        return false;
                }
        }
        return true;
}
bool isWho(char* buf) {
        char who[] = "/who\n";
        for(int i = 0; i <= 4; i++) {
                if(buf[i] != who[i]) return false;
        }
        return true;
}
bool isMessage(char* buf) {
        char tmp[] = "/";
        if(buf[0] == tmp[0]) return false;  // command is start with '/'
        return true;
}
bool isEmptyString(string str) {
        for(int i = 0; i < str.length(); i++) {
                if(str[i] != ' ' && str[i] != '\n') return false;
        }
        return true;
}
string getSpace(int len) {  // return space with length
        string ans = "";
        for(int i = 0; i < len; i++) {
                ans = ans + " ";
        }
        return ans;
}

int main(int argc, char **argv)
{
        int                 i, maxi, maxfd, listenfd, connfd, sockfd;
        int                 nready, client[FD_SETSIZE];
        ssize_t             n;
        fd_set              rset, allset;
        char                buf[MAXLINE];
        socklen_t           clilen;
        struct sockaddr_in  cliaddr, servaddr;
        uint16_t            SERV_PORT = atoi(argv[1]);

        // socket
        if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Socket creation failed\n");
		exit(0);
	} else {
		printf("Socket successfully created\n");
        }

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family      = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port        = htons(SERV_PORT);

        // bind
        if ((bind(listenfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("Socket bind failed\n");
		exit(0);
	} else {
		printf("Socket successfully binded\n");
        }

        // listen
	if ((listen(listenfd, LISTENQ)) != 0) {
		printf("Listen failed\n");
		exit(0);
	} else {
		printf("Server listening\n");
        }
        maxfd = listenfd;
        maxi = -1;
        for (i = 0; i < FD_SETSIZE; i++) client[i] = -1;
        FD_ZERO(&allset);
        FD_SET(listenfd, &allset);

        // record clients' info
        map<int, string> names;  // i from client -> name

        for ( ; ; ) {
                rset = allset;
                nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

                if (FD_ISSET(listenfd, &rset)) {  // new client connection
                        // accept
                        clilen = sizeof(cliaddr);
                        if ( (connfd = accept(listenfd, (SA*) &cliaddr, &clilen)) < 0) {
                                if(errno == EINTR) continue;
                                else {
                                        printf("Server accept failed\n");
                                        exit(0);
                                }   
                        }
#ifdef  NOTDEF
                        printf("new client: %s, port %d\n",
                                        Inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL),
                                        ntohs(cliaddr.sin_port));
#endif

                        for (i = 0; i < FD_SETSIZE; i++)
                                if (client[i] < 0) {
                                        client[i] = connfd;

                                        // 1. welcome message
                                        string currTime = currentDateTime();
                                        string welcomeMsg = currTime + " *** Welcome to the simple CHAT server\n";
                                        write(connfd, welcomeMsg.c_str(), welcomeMsg.length());

                                        string usrName = animals[rand() % animalNum];  // random name
                                        names.insert(pair<int, string>(i, usrName));  // record the user's name
                                        int usrNum = names.size();
                                        welcomeMsg = currTime + " *** Total " + to_string(usrNum) + " users online now. Your name is <" + usrName + ">\n";
                                        write(connfd, welcomeMsg.c_str(), welcomeMsg.length());

                                        // 4.1 display message on the server when user arrival
                                        string serverMsg = "* client connected from ";
                                        struct sockaddr_in local_addr;
                                        socklen_t addr_size = sizeof(local_addr);
                                        getpeername(connfd, (struct sockaddr *)&local_addr, &addr_size);
                                        serverMsg = serverMsg + inet_ntoa(local_addr.sin_addr) + ":" + to_string(local_addr.sin_port);
                                        cout << serverMsg << endl;

                                        // 6.1 display message on all clients when user arrival
                                        string userMsg = currentDateTime() + " *** User <" + usrName + "> has just landed on the server\n";
                                        for(int j = 0; j <= maxi; j++) {
                                                if(client[j] < 0 || i == j) continue;
                                                // write(client[j], userMsg.c_str(), userMsg.length());
                                                if(write(client[j], userMsg.c_str(), userMsg.length()) == -1) {
                                                        cout << "Fail to write when user arrive\n";
                                                        // exit(0);
                                                }
                                        }

                                        break;
                                }
                        if (i == FD_SETSIZE) {
                                cout << "too many clients\n";
                                exit(0);
                        }

                        FD_SET(connfd, &allset);
                        if (connfd > maxfd) maxfd = connfd;
                        if (i > maxi) maxi = i;
                        if (--nready <= 0) continue;  // no more readable descriptors
                }

                for (i = 0; i <= maxi; i++) {  // check all
                        if ( (sockfd = client[i]) < 0) continue;
                        if (FD_ISSET(sockfd, &rset)) {
                                memset(buf, 0, sizeof(buf));
                                n = read(sockfd, buf, MAXLINE);
                                if (n == 0) {
                                        // 4.2 display message on the server when user departure
                                        string serverMsg = "* client ";
                                        struct sockaddr_in local_addr;
                                        socklen_t addr_size = sizeof(local_addr);
                                        getpeername(sockfd, (struct sockaddr *)&local_addr, &addr_size);
                                        serverMsg = serverMsg + inet_ntoa(local_addr.sin_addr) + ":" + to_string(local_addr.sin_port);
                                        serverMsg += " disconnected";
                                        cout << serverMsg << endl;

                                        // 6.2 display message on all clients when user departure, record the left users first
                                        leftUsers.push(names[i]);

                                        close(sockfd);
                                        FD_CLR(sockfd, &allset);
                                        client[i] = -1;
                                        names.erase(i);  // remove key!!
                                } else if (n == -1) {
                                        // cout << "Read fail\n";
                                } else { 
                                        // 2. command </name> + 7.0
                                        if(isName(buf)) {
                                                string cmd = buf;
                                                string newName = cmd.substr(6, cmd.length() - 6);  // *** substr(start, length) ***

                                                // 7.0 extra case!! Can not change the name to empty string
                                                if(isEmptyString(newName)) {
                                                        string errMsg = currentDateTime() + " *** No nickname given\n";
                                                        write(sockfd, errMsg.c_str(), errMsg.length());
                                                        continue;
                                                }

                                                // show the info of changing name by myself
                                                newName = newName.substr(0, newName.length() - 1);  // remove '\n'
                                                string nameInfo = currentDateTime() + " *** Nickname changed to <" + newName + ">\n";
                                                write(sockfd, nameInfo.c_str(), nameInfo.length());
                                                // write to all clients
                                                nameInfo = currentDateTime() + " *** User " + names[i] + " renamed to <" + newName + ">\n";
                                                for(int j = 0; j <= maxi; j++) {
                                                        if(client[j] < 0 || i == j) continue;
                                                        write(client[j], nameInfo.c_str(), nameInfo.length());
                                                }
                                                names[i] = newName;  // record the new name
                                                continue;
                                        }

                                        // 3. command: /who
                                        if(isWho(buf)) {
                                                string dashLine = "--------------------------------------------------\n";
                                                write(sockfd, dashLine.c_str(), dashLine.length());
                                                // print all users
                                                for(int j = 0; j <= maxi; j++) {
                                                        if(client[j] < 0) continue;

                                                        string whoInfo;
                                                        if(i == j) whoInfo = "* " + names[j];
                                                        else whoInfo = "  " + names[j];
                                                        whoInfo = whoInfo + getSpace(23 - whoInfo.length());

                                                        // get IP address and port number
                                                        struct sockaddr_in local_addr;
                                                        socklen_t addr_size = sizeof(local_addr);
                                                        getpeername(client[j], (struct sockaddr *)&local_addr, &addr_size);
                                                        whoInfo = whoInfo + inet_ntoa(local_addr.sin_addr) + ":" + to_string(local_addr.sin_port);
                                                        whoInfo += "\n";
                                                        
                                                        write(sockfd, whoInfo.c_str(), whoInfo.length());
                                                }
                                                write(sockfd, dashLine.c_str(), dashLine.length());
                                                continue;
                                        }

                                        // 5. deliver message
                                        if(isMessage(buf)) {
                                                string msgLeft = buf;
                                                string message = currentDateTime() + " *** User <" + names[i] + "> " + msgLeft;
                                                for(int j = 0; j <= maxi; j++) {
                                                        if(client[j] < 0 || i == j) continue;
                                                        write(client[j], message.c_str(), message.length());
                                                }
                                                continue;
                                        }

                                        // 7. unknown or incomplete command
                                        string cmd = buf;
                                        string errorMsg = currentDateTime() + " *** Unknown or incomplete command <" + cmd.substr(0, cmd.length() - 1) + ">\n";
                                        write(sockfd, errorMsg.c_str(), errorMsg.length());
                                }
                                if (--nready <= 0) break;                          /* no more readable descriptors */
                        }
                }

                // 6.2 display message on all clients when user departure
                // string userMsg = "test hello\n";
                string userMsg = "";
                while(!leftUsers.empty()) {
                        string tmpName = leftUsers.front(); leftUsers.pop();
                        string tmpMsg = currentDateTime() + " *** User <" + tmpName + "> has left the server\n";
                        userMsg += tmpMsg;
                }
                for(int j = 0; j <= maxi; j++) {
                        if(client[j] == -1) continue;
                        send(client[j], userMsg.c_str(), userMsg.length(), MSG_NOSIGNAL);
                }
        }
}