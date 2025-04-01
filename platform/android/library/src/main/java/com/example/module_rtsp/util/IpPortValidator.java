package com.example.module_rtsp.util;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * create by liaohailong
 * 2024/12/8 17:55
 * desc:
 */
public class IpPortValidator {

    // 校验 IP 地址和端口号
    public static boolean isValidIpPort(String input) {
        // 正则表达式，匹配 "IP:端口" 格式
        String regex = "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\:(\\d{1,5})$";

        // 创建模式并进行匹配
        Pattern pattern = Pattern.compile(regex);
        Matcher matcher = pattern.matcher(input);

        if (matcher.matches()) {
            // 获取 IP 和端口
            String ip = input.split(":")[0];
            String portStr = input.split(":")[1];

            // 校验端口号范围是否有效 (0-65535)
            int port = Integer.parseInt(portStr);
            return port >= 0 && port <= 65535;
        }

        return false;
    }

}
