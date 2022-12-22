/*
Student No.:
109550167
Student Name:
陳唯文
Email:
ben167.cs09@nycu.edu.tw
SE tag:
xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/
#define FUSE_USE_VERSION 30
#include <bits/stdc++.h>
#include <fuse.h>

using namespace std;

int my_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
int my_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi);
int my_getattr(const char *path, struct stat *st);
void handle_tar();
static struct fuse_operations op;

struct header {
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
    int pos;
};

ifstream tar("test.tar", ios::binary);
map <string, vector<string>> dir;
map <string, attr*> mpa;

int main(int argc, char** argv)
{  
    handle_tar();
	//FUSE
    memset(&op, 0, sizeof(op));
    op.getattr = my_getattr;
    op.readdir = my_readdir;
    op.read = my_read;
    return fuse_main(argc, argv, &op, NULL);
}

void handle_tar()
{
    tar.seekg(0, tar.end);
    int end = tar.tellg();
    tar.seekg(0, tar.beg);
    while (true) {
		header *hdr = new header;
        attr *a = new attr;

		tar.read((char *) hdr, 512);

		if (hdr->name[0] == NULL)
			break;

        //attr
        a->gid = stoi(hdr->gid, 0, 8);
        a->uid = stoi(hdr->uid, 0, 8);
        a->mtime = stoi(hdr->mtime, 0, 8);
        a->size = stoi(hdr->size, 0, 8);

        a->pos = tar.tellg();
        //file
		if(!strcmp(hdr->typeflag, "0"))
		{
            a->mode = S_IFREG | stoi(hdr->mode, 0, 8);
            string tmp = "/";
            tmp.append(string(hdr->name));
            if(mpa.count(tmp) > 0)
            {
                if(mpa[tmp]->mtime < a->mtime)
                    mpa[tmp] = a;
            }
            else
            {
                int pos = string(hdr->name).find_last_of('/');
                if(pos != -1)
                    dir[string(hdr->name).substr(0, pos)].push_back(string(hdr->name).substr(pos+1));
                else
                    dir["root"].push_back(string(hdr->name));
                mpa[tmp] = a;
            }
		}
        //dir
        else
        {
            a->mode = S_IFDIR | stoi(hdr->mode, 0, 8);
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
        tar.seekg((stoi(hdr->size, 0, 8) + 511) & ~511, ios::cur);
	}
    attr *roota = new attr;
    roota->mode = S_IFDIR | 0444;
    mpa["/"] = roota;
}

int my_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    if (path[1] == '\0')
    {
        for(int i=0; i<dir["root"].size(); i++)
            filler(buffer, dir["root"][i].c_str(), NULL, 0);
        return 0;
    }

    string p(path);
    p.erase(p.begin());

    for(int i=0; i<dir[p].size(); i++)
        filler(buffer, dir[p][i].c_str(), NULL, 0);

    return 0;
}

int my_getattr(const char *path, struct stat *st)
{
    if(mpa.count(path) == 0)
        return -2;
    
    st->st_gid = mpa[path]->gid;
    st->st_uid = mpa[path]->uid;
    st->st_mode = mpa[path]->mode;
    st->st_size = mpa[path]->size;
    st->st_mtime = mpa[path]->mtime;
    // printf("%s: %d\n", path, mpa[path]->mode);
    printf("%s: %d\n", path, mpa[path]->mtime);
    return 0;
}

int my_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
    size_t sz = size;
	if (sz > mpa[path]->size - offset)
		sz = mpa[path]->size - offset;
    tar.seekg(mpa[path]->pos + offset);
    tar.read(buffer, sz);
    return sz;
}