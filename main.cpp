#include<iostream>
#include <fstream>
#include <string>
#include<opencv2/opencv.hpp>
#include <iosfwd>
#include <bitset>
#include <sstream>

typedef struct HafuTree{
    int weight;
    int data;
    HafuTree* LeftChild;
    HafuTree* RightChild;
    HafuTree* Parent;
}*HuffTree,HTNode;

void insert(HafuTree* node, const std::string& code, uchar data) {
    if (code.empty()) {
        node->data = data;
    } else {
        if (code[0] == '0') {
            if (node->LeftChild == nullptr) {
                node->LeftChild = new HafuTree();
            }
            insert(node->LeftChild, code.substr(1), data);
        } else {
            if (node->RightChild == nullptr) {
                node->RightChild = new HafuTree();
            }
            insert(node->RightChild, code.substr(1), data);
        }
    }
}
//创建哈夫曼树
void createHafuTree(HuffTree &root){
    int m;
    std::cout<<"请输入叶子节点的个数：";
    std::cin>>m;
    root = new HTNode[2*m-1];
    //初始化叶子节点
    for(int i=0;i<m;i++){
        std::cout<<"请输入第"<<i+1<<"个叶子节点的权值：";
        std::cin>>root[i].weight;
        root[i].data=root[i].weight;
        root[i].LeftChild = NULL;
        root[i].RightChild = NULL;
        root[i].Parent = NULL;
    }
    //初始化非叶子节点
    for(int i=m;i<2*m-1;i++){
        root[i].LeftChild = NULL;
        root[i].RightChild = NULL;
        root[i].Parent = NULL;
        root[i].weight = 0;
    }
    //对叶子节点进行排序
    for(int i=0;i<m-1;i++){
        for(int j=i;j<m;j++){
            if(root[i].weight>root[j].weight){
                HTNode temp = root[i];
                root[i] = root[j];
                root[j] = temp;
            }
        }
    }
    //构造哈夫曼树
    for(int i=0;i<m-1;i++){
        int min1_index = 0, min2_index = 0;
        int min1 = 1000000, min2 = 1000000;
        for(int j=i;j<m+i;j++){
            if(root[j].Parent==NULL){
                if(root[j].weight<min1){
                    min2 = min1;
                    min2_index = min1_index;
                    min1 = root[j].weight;
                    min1_index = j;
                }else if(root[j].weight<min2){
                    min2 = root[j].weight;
                    min2_index = j;
                }
            }
        }
        root[min1_index].Parent = &root[m+i];
        root[min2_index].Parent = &root[m+i];
        root[m+i].weight = root[min1_index].weight + root[min2_index].weight;
        root[m+i].LeftChild = &root[min1_index];
        root[m+i].RightChild = &root[min2_index];

    }
    root = &root[2*m-2];
}

//创建二叉树
void createHafuTree_Noinput(HuffTree &root,std::vector<cv::Point2d> pixels){
    int m;
    m = pixels.size();
    std::cout<<"叶子节点的个数："<<m<<std::endl;
    root = new HTNode[2*m-1];
    //初始化叶子节点
    int i=0;
    for(auto pixel:pixels){
        root[i].weight=pixel.y;
        root[i].data=pixel.x;
        root[i].LeftChild = NULL;
        root[i].RightChild = NULL;
        root[i].Parent = NULL;
        i++;
    }
    //初始化非叶子节点
    for(int i=m;i<2*m-1;i++){
        root[i].LeftChild = NULL;
        root[i].RightChild = NULL;
        root[i].Parent = NULL;
        root[i].weight = 0;
        root[i].data = 0;
    }
    //对叶子节点进行排序
    for(int i=0;i<m-1;i++){
        for(int j=i;j<m;j++){
            if(root[i].weight>root[j].weight){
                HTNode temp = root[i];
                root[i] = root[j];
                root[j] = temp;
            }
        }
    }
    //构造哈夫曼树
    for(int i=0;i<m-1;i++){
        int min1_index = 0, min2_index = 0;
        int min1 = 1000000, min2 = 1000000;
        for(int j=i;j<m+i;j++){
            if(root[j].Parent==NULL){
                if(root[j].weight<min1){
                    min2 = min1;
                    min2_index = min1_index;
                    min1 = root[j].weight;
                    min1_index = j;
                }else if(root[j].weight<min2){
                    min2 = root[j].weight;
                    min2_index = j;
                }
            }
        }
        root[min1_index].Parent = &root[m+i];
        root[min2_index].Parent = &root[m+i];
        root[m+i].weight = root[min1_index].weight + root[min2_index].weight;
        root[m+i].LeftChild = &root[min1_index];
        root[m+i].RightChild = &root[min2_index];

    }
    root = &root[2*m-2];
}

//生成哈夫曼编码
void generateHuffmanCodes(HuffTree root, std::string code, std::map<int, std::string>& codes) {
    if (root->LeftChild) {
        generateHuffmanCodes(root->LeftChild, code + "0", codes);
    }
    if (root->RightChild) {
        generateHuffmanCodes(root->RightChild, code + "1", codes);
    }
    if (!root->LeftChild && !root->RightChild) {
        codes[root->data] = code;
    }
}

//画出哈夫曼树
void DrawHafuNode(HafuTree *hafu,cv::Mat &img,cv::Point root,int linelength)
{
    cv::Point left(root.x-linelength,root.y+100);
    cv::Point right(root.x+linelength,root.y+100);
    if(hafu){
        cv::circle(img,root,16,cv::Scalar(255,0,0),1);
        //填充根的数据
        int i = hafu->weight;
        cv::putText(img,std::to_string(i),cv::Point(root.x-5,root.y+5),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(0,0,255),1.5);
        //画出左右子树
        if(hafu->LeftChild){
        cv::line(img,cv::Point(root.x-13,root.y+13),cv::Point(left.x+13,left.y-13),cv::Scalar(0,255,0),2);
        cv::putText(img,"0",cv::Point((root.x+left.x)/2.0,(root.y+left.y)/2.0-5),cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,0,0),1);
        DrawHafuNode(hafu->LeftChild,img,left,linelength-16);
        }
        if(hafu->RightChild){
        cv::line(img,cv::Point(root.x+13,root.y+13),cv::Point(right.x-13,right.y-13),cv::Scalar(0,255,0),2);
        cv::putText(img,"1",cv::Point((root.x+right.x)/2.0,(root.y+right.y)/2.0-5),cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar(0,0,0),1);
        DrawHafuNode(hafu->RightChild,img,right,linelength-16);
        }
    }
}

//画出哈夫曼树
void DrawHafuTree(HafuTree *T){ 
    cv::Mat img(1000,1000,CV_8UC3,cv::Scalar(255,255,255));
    cv::Point root(500,50);
    int linelength = 100;
    DrawHafuNode(T,img,root,linelength);
    std::map<int, std::string> huffmanCodes;
    generateHuffmanCodes(T, "", huffmanCodes);
    int show_y=100;
    cv::putText(img,"HuffmanCodes:",cv::Point(16,show_y-16),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(18,0,255),1);
    for (const auto& pair : huffmanCodes) {
        std::cout << pair.first << ": " << pair.second << std::endl;
        cv::putText(img,std::to_string(pair.first)+":"+pair.second,cv::Point(16,show_y),cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(18,0,255),1);
        show_y+=16;
    }
    cv::imshow("hafuTree",img);
    cv::waitKey(1000);
}

//编码哈夫曼树
void encodeHaffTree(HuffTree root, const std::string& inputstring, const std::string& path = "../encode.txt") {
    std::map<int, std::string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);
     std::ofstream outputFile(path);
    if (!outputFile.is_open()) {
        std::cerr << "无法打开文件" << std::endl;
        return;
    }
    //
    std::istringstream inputStream(inputstring);
    std::vector<int> numbers;
    int num =0;
    while (inputStream >> num) {
        numbers.push_back(num);
    }
    //
    for(int i=0;i<numbers.size();i++){
       auto it = huffmanCodes.find(numbers[i]);
        if (it != huffmanCodes.end()) {
            outputFile << it->second;
        } else {
            std::cerr << "值 " << numbers[i] << " 没有对应的哈夫曼编码" << std::endl;
        }
    }
    outputFile.close();
}

//译码哈夫曼树
void decodeHaffTree(HuffTree root,std::string path1="../encode.txt",std::string path2="../decode.txt") {
    std::string inputFile = path1;
    std::string outputFile = path2;
    FILE *input = fopen(inputFile.c_str(), "r");
    FILE *output = fopen(outputFile.c_str(), "w");
    char bit;
    HuffTree currentNode = root;

    if (!input || !output) {
        std::cerr << "无法打开文件" << std::endl;
        return;
    }

    while (fscanf(input, "%c", &bit) != EOF) {
        if (bit == '0') {
            currentNode = currentNode->LeftChild;
        }
        else if (bit == '1') {
            currentNode = currentNode->RightChild;
        }
        if (!currentNode->LeftChild && !currentNode->RightChild) {
            fprintf(output,"%d ", currentNode->weight);
            currentNode = root;
        }
    }
    fclose(input);
}

//图像压缩
void imgCompression(std::string imgpath) {
    HafuTree *root;
    // 为了方便实现，将图像转换为灰度图像
    cv::Mat img = cv::imread(imgpath, cv::IMREAD_GRAYSCALE);
    FILE* file = fopen((imgpath + "_Compress").c_str(), "wb");
    std::vector<cv::Point2d> pixels;
    int realpixels[256] = {0};
    // 获取图像的行数和列数
    int rows = img.rows;
    int cols = img.cols;
    // 遍历图像的每个像素
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int pixel = img.at<uchar>(i, j);
            realpixels[pixel]++;
        }
    }
    // 将像素值存入set中
    for (int i = 0; i < 255; i++) {
        if (realpixels[i] != 0) {
            pixels.push_back(cv::Point2d(i, realpixels[i]));
        }
    }
    // 创建哈夫曼树
    createHafuTree_Noinput(root, pixels);
    // 生成哈夫曼编码
    std::map<int, std::string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);
    // 保存哈夫曼编码
    std::string outputFile = imgpath + "_huffmanCodes.txt";
    std::ofstream output(outputFile);
    if (!output.is_open()) {
        std::cerr << "Error opening Huffman codes file." << std::endl;
        return;
    }
    for (const auto& pair : huffmanCodes){
        output << pair.first << ":" << pair.second << std::endl;
    }
    // 创建一个字节缓冲区和一个计数器
    unsigned char buffer = 0;
    int bit_count = 0;
    // 写入图像的行数和列数
    fwrite(&rows, sizeof(rows), 1, file);
    fwrite(&cols, sizeof(cols), 1, file);
    fputc('\n', file);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int pixel = img.at<uchar>(i, j);
            std::string huff = huffmanCodes[pixel];
            for (char bit : huff) {
                // 将bit添加到缓冲区
                buffer = (buffer << 1) | (bit == '1');
                bit_count++;
                // 如果缓冲区已满，将其写入文件
                if (bit_count == 8) {
                    fwrite(&buffer, sizeof(buffer), 1, file);
                    buffer = 0;
                    bit_count = 0;
                }
            }
        }
    }

    // 如果缓冲区中还有剩余的bit，将其写入文件
    if (bit_count > 0) {
        buffer <<= (8 - bit_count);
        fwrite(&buffer, sizeof(buffer), 1, file);
    }
    fclose(file);
}


//图像解压
void imgDeCompression(const std::string& imgCompressionpath, const std::string& huffmanCodespath) {
    std::ifstream input(huffmanCodespath);
    std::map<int ,std::string> huffmanCodes;
    std::map<int ,std::string> huffmanCodes2;
    FILE *file =std::fopen(imgCompressionpath.c_str(), "rb");
    //  读取图像的行数和列数
    int rows; 
    int cols; 
    fread(&rows, sizeof(rows), 1, file);
    fread(&cols,sizeof(cols),1,file); 
    std::cout<<rows<<" "<<cols<<std::endl;
    //异常处理
    if (!input.is_open()) {
        std::cerr << "Error opening Huffman codes file." << std::endl;
        return;
    }
    
    std::string line;

    //读取哈夫曼编码
    while (std::getline(input, line)) {
    std::istringstream iss(line);
    int key;
    std::string key_1;
    std::string value;
    if (std::getline(iss, key_1, ':') && std::getline(iss, value)) {
        key = std::stoi(key_1);  // 将字符串转换为整数
        huffmanCodes.insert({key,value});
        } 
    }
    // 创建一个空白图像
    cv::Mat img_decompression(rows, cols, CV_8U, cv::Scalar(0));
    HuffTree root=new HTNode;
    root->data = 0;
    root->LeftChild = NULL;
    root->RightChild = NULL;
    root->Parent = NULL;
    //根据哈夫曼编码创建哈夫曼树
    for(auto code: huffmanCodes){
        insert(root,code.second,code.first);
    }
    //输出哈夫曼树
    generateHuffmanCodes(root, "", huffmanCodes2);
    for(auto code: huffmanCodes2){
        std::cout<<code.first<<":"<<code.second<<std::endl;
    }
    std::vector<bool> bitVector;
    //
    
    //逐个bit读取
    char c;
    // 读取文件中的每个字节
    while (fread(&c, sizeof(c), 1, file) == 1) {
        std::bitset<8> bits(c);
        for (int i = 7; i >= 0; --i) {
            bitVector.push_back(bits[i]);
        }
    }
    HuffTree root2=root;

    //根据哈夫曼编码解码
    int vectorIndex = 0;

    // 根据哈夫曼编码解码
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            while (root2->LeftChild || root2->RightChild) {
                if (bitVector[vectorIndex] == 0) {
                    root2 = root2->LeftChild;
                } else {
                    root2 = root2->RightChild;
                }
                ++vectorIndex;
            }
            // 找到叶子节点，将值存储到图像中
            img_decompression.at<uchar>(i, j) = static_cast<uchar>(root2->data);
            // 重置为根节点
            root2 = root;
        }
    }

    cv::imwrite(imgCompressionpath+"_decompression.jpg",img_decompression);
    input.close();
    fclose(file);
}

//图像压缩

int main(){
    HuffTree root;
    createHafuTree(root);
    DrawHafuTree(root);
    std::map<int, std::string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);
    getchar();
    std::string encodestring;
    std::cout<<"输入编码数据,用空格隔开:";
    //windows下请改成\r
    std::getline(std::cin, encodestring);
    encodeHaffTree(root,encodestring);
    decodeHaffTree(root);
    imgCompression("../运行截图 .png");
    imgDeCompression("../运行截图.png_Compress","../运行截图.png_huffmanCodes.txt");
}