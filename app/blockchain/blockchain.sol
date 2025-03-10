// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

contract CBFVerifier {
    struct CountingBloomFilter {
        uint256[] bits; // 计数布隆过滤器的数组表示
    }

    mapping(string => CountingBloomFilter) public CBFList; // 存储 Bloom Filter

    // 添加 CBF
    function addCBF(string memory key, uint256[] memory data) public {
        CBFList[key] = CountingBloomFilter(data);
    }

    // 计算 BCBF 并验证 RCBF
    function verifyCBF(uint256[] memory RCBF, string[] memory searchTokens) public view returns (bool) {
        uint256[] memory BCBF = new uint256[](RCBF.length); // 初始化 BCBF

        // 遍历 searchTokens 累加对应 CBFList 数据
        for (uint256 i = 0; i < searchTokens.length; i++) {
            CountingBloomFilter storage cbf = CBFList[searchTokens[i]];
            for (uint256 j = 0; j < cbf.bits.length; j++) {
                BCBF[j] += cbf.bits[j]; // 累加 Bloom 计数
            }
        }

        // 比较 RCBF 和计算出的 BCBF
        return keccak256(abi.encodePacked(BCBF)) == keccak256(abi.encodePacked(RCBF));
    }
}
