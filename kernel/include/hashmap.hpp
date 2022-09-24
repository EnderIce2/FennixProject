#pragma once

template <typename K, typename V>
class HashNode
{
public:
    V Value;
    K Key;

    HashNode(K Key, V Value)
    {
        this->Value = Value;
        this->Key = Key;
    }
};

template <typename K, typename V>
class HashMap
{
    int HashMapSize;
    int HashMapCapacity;
    HashNode<K, V> **Nodes;
    HashNode<K, V> *DummyNode;

public:
    HashMap()
    {
        HashMapCapacity = 20;
        HashMapSize = 0;
        Nodes = new HashNode<K, V> *[HashMapCapacity];
        for (int i = 0; i < HashMapCapacity; i++)
            Nodes[i] = nullptr;
        DummyNode = new HashNode<K, V>(-1, -1);
    }

    int HashCode(K Key) { return Key % HashMapCapacity; }

    void AddNode(K Key, V Value)
    {
        HashNode<K, V> *tmp = new HashNode<K, V>(Key, Value);
        int Index = HashCode(Key);

        while (Nodes[Index] != nullptr && Nodes[Index]->Key != Key && Nodes[Index]->Key != -1)
        {
            Index++;
            Index %= HashMapCapacity;
        }

        if (Nodes[Index] == nullptr || Nodes[Index]->Key == -1)
            HashMapSize++;
        Nodes[Index] = tmp;
    }

    V DeleteNode(int Key)
    {
        int Index = HashCode(Key);

        while (Nodes[Index] != nullptr)
        {
            if (Nodes[Index]->Key == Key)
            {
                HashNode<K, V> *tmp = Nodes[Index];
                Nodes[Index] = DummyNode;
                HashMapSize--;
                return tmp->Value;
            }
            Index++;
            Index %= HashMapCapacity;
        }
        return nullptr;
    }

    V Get(int Key)
    {
        int Index = HashCode(Key);
        int Iterate = 0;

        while (Nodes[Index] != nullptr)
        {

            if (Iterate++ > HashMapCapacity)
                return nullptr;

            if (Nodes[Index]->Key == Key)
                return Nodes[Index]->Value;
            Index++;
            Index %= HashMapCapacity;
        }

        return nullptr;
    }

    int Size() { return HashMapSize; }
    bool IsEmpty() { return HashMapSize == 0; }
};
