#include <iostream>
#include <stdlib.h>
#include <string>
#include <list>
#include <sstream>
#include <iomanip>
#include <vector>
#include <math.h>

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
    int blockNo, levels;
    const int HASHSIZE = 2 * SHA256_DIGEST_LENGTH;

public:
    MerkleTree()
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

    bool BuildTree(const string* datas, int size)
    {
        vector<Node*> listChild, listParent;
        Node* leftN = nullptr, * rightN = nullptr;
        size_t tmp = 0;
        while (tmp < size)
        {
            Node* n = new Node;
            n->data = datas[tmp];
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

            //不平衡时
            if (leftN)
            {
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
        // 若原来是完全二叉树
        if (blockNo == pow(2, levels))
        {
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
        //更新哈希值
        UpdateHash(leftN);
        return false;
    }

    string ProveBlock(int n)
    {
        string proof("");
        if (n > blockNo)
        {
            cout << "Not found!" << endl;
            return proof;
        }

        int tempN = n;
        Node* tree = root;
        for (int i = levels; i >= 0; i--)
        {
            if (tempN < pow(2, i - 1)) // Go left
            {
                if (tree->left)
                    tree = tree->left;
                else
                    cout << "Found: " << tree->hashvalue << endl;
            }
            else
            {
                tempN -= pow(2, i - 1);
                if (tree->right)
                    tree = tree->right;
                else
                    cout << "Found: " << tree->hashvalue << endl;
            }
        }

        bool bFirst = true;
        Node* leftN = tree, * parentN = leftN->parent, * rightN = nullptr;
        while (parentN)
        {
            if (parentN->left == leftN)
            {
                rightN = parentN->right;
                if (bFirst)
                {
                    proof += "L:";
                    proof += leftN->data;
                    bFirst = false;
                }

                proof += "R:";
                proof += rightN->hashvalue;
            }
            else
            {
                rightN = leftN;
                leftN = parentN->left;
                if (bFirst)
                {
                    proof += "R:";
                    proof += rightN->data;
                    bFirst = false;
                }

                proof += "L:";
                proof += leftN->hashvalue;
            }

            leftN = parentN;
            parentN = parentN->parent;
        }

        return proof;
    }

};


int main() {

    MerkleTree tree;

    return 0;
}

