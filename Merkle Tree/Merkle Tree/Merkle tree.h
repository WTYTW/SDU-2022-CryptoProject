#include <iostream>
#include <stdlib.h>
#include <string>
#include <list>
#include <sstream>
#include <iomanip>
#include <vector>

#include <openssl/sha.h>

using namespace std;

struct Node {
	int ID;
	string data;
    string hashvalue;
	Node* left, * right, * parent;
};

class MerkleTree {
	Node* root;
	int blockSize, blockNo, levels;
	const int HASHSIZE = 2 * SHA256_DIGEST_LENGTH;

public:
	MerkleTree(int bSize) : blockSize(bSize)
	{
		levels = 0;
		blockNo = -1;
	}

	string SHA256(const string strIn)
	{
		unsigned char hash[SHA256_DIGEST_LENGTH];
		SHA256_CTX sha256;
		SHA256_Init(&sha256);
		SHA256_Update(&sha256, strIn.c_str(), strIn.size());
		SHA256_Final(hash, &sha256);
		stringstream ss;
		for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
			ss << hex << setw(2) << setfill('0') << (int)hash[i];
		return ss.str();
	}

    bool BuildTree(const string* datas,int size)
    {
        vector<Node*> listChild, listParent;
        Node* leftN = nullptr, * rightN = nullptr;
        char *buffer=new char[blockSize + 1];
        buffer[blockSize] = '\0';
        size_t tmp = 0;
        while (tmp<size)
        {
            Node* n = new Node;
            n->data = (char*)datas[tmp].c_str();
            n->hashvalue = SHA256(n->data);
            n->ID = ++blockNo;
            n->left = n->right = n->parent = nullptr;
            listChild.push_back(n);
            tmp++;
        }

        do
        {
            levels++;
        } while (pow(2, levels) <= blockNo);

        // And, build the tree over the blocks stored in the list.
        do
        {
            auto iterP = listParent.begin();
            auto doneP = listParent.end();
            while (iterP != doneP)
                listChild.push_back(*iterP++);
            listParent.clear();

            auto iter = listChild.begin();
            auto done = listChild.end();
            leftN = rightN = nullptr;
            while (iter != done)
            {
                //cout << "list: " << child->hashValue << endl;
                if (!leftN)
                    leftN = *iter++;
                else
                {
                    rightN = *iter++;
                    Node* parentN = new Node;
                    parentN->data = leftN->data;
                    parentN->hashvalue = SHA256(leftN->hashvalue + rightN->hashvalue);
                    parentN->left = leftN;
                    parentN->right = rightN;
                    parentN->parent = nullptr;
                    parentN->ID = -1;
                    listParent.push_back(parentN);
                    leftN->parent = rightN->parent = parentN;
                    leftN = rightN = nullptr;
                }
            }

            //cout << endl;
            if (leftN)
            {
                // Make a new NULL node:
                rightN = new Node;
                rightN->hashvalue = "";
                rightN->ID = -1;
                rightN->left = rightN->right = nullptr;

                Node* parentN = new Node;
                parentN->data = leftN->data;
                parentN->hashvalue = SHA256(leftN->hashvalue + rightN->hashvalue);
                parentN->left = leftN;
                parentN->right = rightN;
                parentN->parent = nullptr;
                parentN->ID = -1;
                listParent.push_back(parentN);
                leftN->parent = rightN->parent = parentN;
                leftN = rightN = nullptr;
            }

            listChild.clear();
        } while (listParent.size() > 1);

        root = listParent[0];
        return true;
    }

    void UpdateHash(Node* leftN)
    {
        Node* parentN = leftN->parent, * rightN = nullptr;
        while (parentN)
        {
            if (parentN->left == leftN)
                rightN = parentN->right;
            else
            {
                rightN = leftN;
                leftN = parentN->left;
            }

            parentN->hashvalue = SHA256(leftN->hashvalue + rightN->hashvalue);
            leftN = parentN;
            parentN = parentN->parent;
        }
    }

    bool Append(string strBlock)
    {
        if (!strBlock.length())
        {
            cout << "Error: Empty block!" << endl;
            return false;
        }

        blockNo++;
        Node* parentN = nullptr, * leftN = nullptr, * rightN = nullptr;
        // If its a complete tree, add a new subtree and add one more level.
        if (blockNo == pow(2, levels))
        {
            cout << "1: blockNo: " << blockNo << "\tlevels: " << levels << endl;
            // Make a new root node.
            parentN = new Node;
            parentN->data = parentN->hashvalue = "";
            parentN->ID = -1;
            parentN->left = root;
            parentN->right = parentN->parent = nullptr;
            root->parent = parentN;
            root = parentN;

            parentN = new Node;
            parentN->data = parentN->hashvalue = "";
            parentN->ID = -1;
            parentN->left = parentN->right = nullptr;
            parentN->parent = root;
            root->right = parentN;

            // Make levels number of null nodes to make a new path.
            for (int i = 0; i < levels; i++)
            {
                leftN = new Node;
                leftN->data = "";
                leftN->hashvalue = SHA256("NULL");
                leftN->ID = -1;
                leftN->left = leftN->right = nullptr;

                rightN = new Node;
                rightN->ID = -1;
                rightN->data = "";
                rightN->hashvalue = SHA256("NULL");
                rightN->left = rightN->right = nullptr;

                parentN->left = leftN;
                parentN->right = rightN;
                leftN->parent = rightN->parent = parentN;
                parentN = leftN;
            }

            leftN->ID = blockNo;
            leftN->hashvalue = SHA256(strBlock);
            levels++;
        }
        else
        {
            parentN = root;
            int tempN = blockNo;
            if (blockNo % 2)
            {
                cout << "2: blockNo: " << blockNo << endl;
                for (int i = levels - 1; i >= 0; i--)
                {
                    if (tempN < pow(2, i)) // Go left
                        parentN = parentN->left;
                    else
                    {
                        tempN -= pow(2, i);
                        parentN = parentN->right;
                    }
                }

                leftN = parentN;
                leftN->ID = blockNo;
                leftN->hashvalue = SHA256(strBlock);
            }
            else
            {
                cout << "3: blockNo: " << blockNo << endl;
                int i = levels;
                for (; i >= 0; i--)
                {
                    if (tempN < pow(2, i - 1)) // Go left
                        if (parentN->left)
                            parentN = parentN->left;
                        else
                            break;
                    else
                    {
                        tempN -= pow(2, i - 1);
                        if (parentN->right)
                            parentN = parentN->right;
                        else
                            break;
                    }
                }

                // Now add the required part of the path:
                while (i > 0)
                {
                    leftN = new Node;
                    leftN->ID = -1;
                    leftN->data = leftN->hashvalue = "";
                    leftN->left = leftN->right = nullptr;

                    rightN = new Node;
                    rightN->ID = -1;
                    rightN->data = "";
                    rightN->hashvalue = SHA256("NULL");
                    rightN->left = rightN->right = nullptr;

                    parentN->left = leftN;
                    parentN->right = rightN;
                    leftN->parent = rightN->parent = parentN;
                    parentN = leftN;
                    i--;
                }

                leftN->ID = blockNo;
                leftN->hashvalue = SHA256(strBlock);
            }
        }

        // Now update the hash values from buttom up.
        UpdateHash(leftN);
        return false;
    }

};

