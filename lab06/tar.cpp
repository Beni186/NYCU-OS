#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <fuse.h>
#include <map>
#include <iomanip>

using namespace std;

struct Attr {
	int uid, gid;
	string path;
	size_t size;
	bool is_dir;
	int mtime;
	int mode;
	int pos;
};

struct tar_header {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char checksum[8];
	char typeflag[1];
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
	char pad[12];
};

struct attr{
    int uid;
    int gid;
    int mtime;
    int mode;
    int size;
};

/* Open file in global scope :) */
ifstream tar("test.tar", ios::binary);
hash<string> hasher;
vector <string> vs;
map <string, attr*> mpa;
map <string, vector<string>> dir;
map <string, string> content;

const string ROOT = "/";

int main()
{
    vector<tar_header*> v;
    // if (!tar) {
	// 	cerr << "Cannot open test.tar.\n";
	// 	return 1;
	// }

	/* Read tar to my own structure */

	while (true) {
		tar_header *hdr = new tar_header;
        attr *a = new attr;

		tar.read((char *) hdr, 512);

		if (hdr->name[0] == '\0')
			break;

        //attr
        a->gid = stoi(hdr->gid, 0, 8);
        a->uid = stoi(hdr->uid, 0, 8);
        a->mode = stoi(hdr->mode, 0, 8);
        a->mtime = stoi(hdr->mtime, 0, 8);
        a->size = stoi(hdr->size, 0, 8);

        v.push_back(hdr);
		int aa = tar.tellg();
        //file
		if(!strcmp(hdr->typeflag, "0"))
		{
            //content
			char *file_contents = new char[stoi(hdr->size, 0, 8)];	
			tar.read(file_contents, stoi(hdr->size, 0, 8));
			content[hdr->name] = string(file_contents);	
            //dir
            int pos = string(hdr->name).find_last_of('/');
            if(pos != -1)
                dir[string(hdr->name).substr(0, pos)].push_back(string(hdr->name).substr(pos+1));
            else
                dir["root"].push_back(string(hdr->name));
            string tmp = "/";
            mpa[tmp.append(string(hdr->name))] = a;
			cout<<tmp<<endl;
		}
        else
        {
            string tmp = hdr->name;
			tmp.pop_back();
            if(tmp.find_first_of('/') == -1)
				dir["root"].push_back(tmp);

            else
            {
                int pos = tmp.find_last_of('/');
                dir[tmp.substr(0, pos)].push_back(tmp.substr(pos+1));
            }
            string tmp2 = "/";
            mpa[tmp2.append(tmp)] = a;
        }
		tar.seekg(aa);
        tar.seekg((stoi(hdr->size, 0, 8) + 511) & ~511, ios::cur);
	}
    for(int i=0; i<v.size(); i++)
        cout<<"filename: "<<setw(32)<<v[i]->name<<" mode: "<<stoi(v[i]->mode, 0, 8)<<"\n";
	// for(int i=0; i<vs.size(); i++)
	// 	cout<<"content:\n"<<vs[i]<<'\n';
	// for(int i=0; i < dir["root"].size(); i++)
	// 	cout<<dir["root"][i]<<"\n";
	// for(auto it : dir)
	// {
	// 	cout<<it.first<<endl;
	// 	for(int i=0; i<dir[it.first].size(); i++)
	// 		cout<<dir[it.first][i]<<" ";
	// 	cout<<'\n';
	// }
	for(auto it : mpa)
	{
		// const char *buffer = content[it.first].c_str();
		// cout<<it.first<<"\n"<<buffer<<endl;
		cout<<it.first<<"\n";
	}
}
