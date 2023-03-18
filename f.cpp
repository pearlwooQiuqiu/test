/*

11. 子串匹配
给你一个字符串s和一个字符串p，最少去掉s中的多少个字符，才能使得p是s的子串？

示例1：
输入：
s=”axb”
p=”ab”
输出：
1



给定一个非负整数数组 nums ，你最初位于数组的 第一个下标 。
数组中的每个元素代表你在该位置可以跳跃的最大长度。
判断你是否能够到达最后一个下标。

示例 1：
输入：nums = [2,3,1,1,4]
输出：true
解释：可以先跳 1 步，从下标 0 到达下标 1, 然后再从下标 1 跳 3 步到达最后一个下标。
示例 2：

输入：nums = [3,2,1,0,4]
输出：false
解释：无论怎样，总会到达下标为 3 的位置。但该下标的最大跳跃长度是 0 ， 所以永远不可能到达最后一个下标。
 
提示：
1 <= nums.length <= 3 * 104
0 <= nums[i] <= 105




获取字符串中所有可用的IPv4列表
输入
25525511233
结果
255.255.11.233
255.255.112.33

回溯

*/
#include <iostream>
#include <string>
#include <vector>
#include <stack>
using namespace std;

vector<int> segIP(4,0);
vector<string> res;

void dfs(string &s,int s_id,int cur_idx){
	if(s_id == 4){//IP地址到第四段了
		if(cur_idx == s.size()){ //遍历完了所有的字符
			string tmp;
			for(int i=0;i<4;i++){
				tmp += to_string(segIP[i]);
				if(i!=3) tmp+=".";
			}
			res.push_back(move(tmp));
		}
		return;
	}
	//遍历完所有字符，但是IP并未到第四段 非法IP，返回
	if(cur_idx == s.size()) return;
	//有0情况
	if(s[cur_idx]=='0'){
		segIP[s_id]=0;
		dfs(s,s_id+1,cur_idx+1);
	}
	//无前导0情况
	int sub_ip=0;
	for(int end=cur_idx;end<s.size();end++){
		sub_ip = sub_ip*10+(s[end]-'0');
		if(sub_ip>0 && sub_ip<256){
			segIP[s_id]=sub_ip;
			dfs(s,s_id+1,end+1);
		}
		else break;
	}
}


int main(){
	string str;
	
	cin>>str;
	dfs(str,0,0);
	for(int i=0;i<res.size();i++){
		cout<<res[i]<<endl;
	}

	return 0;
}