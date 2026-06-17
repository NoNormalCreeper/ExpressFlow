#!/usr/bin/env python3
import argparse
from pathlib import Path


def write_lines(path, lines):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def parcel_id(index):
    base_ms = 1781600000000
    return f"P{base_ms + index * 180123}-{index}"


def parcel(index,
           sender,
           receiver,
           description,
           sent_at,
           received_at,
           fee,
           status,
           item_type,
           amount,
           courier,
           picked_at):
    return (
        f"{parcel_id(index)}|{sender}|{receiver}|{description}|{sent_at}|"
        f"{received_at}|{fee}|{status}|{item_type}|{amount:.6f}|{courier}|"
        f"{picked_at}"
    )


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--data-dir", default="data_stage3")
    args = parser.parse_args()

    data_dir = Path(args.data_dir)

    users = [
        "u1|张三|13800000001|123456|北京市昌平区南丰路1号|300.00",
        "u2|李四|13800000002|123456|北京市海淀区西土城路2号|120.00",
        "u3|王五|13800000003|123456|上海市浦东新区世纪大道3号|90.00",
        "u4|赵六|13800000004|123456|广州市天河区体育西路4号|75.00",
        "u5|钱七|13800000005|123456|深圳市南山区科技园5号|45.00",
        "u6|孙八|13800000006|123456|杭州市西湖区文三路6号|60.00",
        "u7|周九|13800000007|123456|南京市鼓楼区中央路7号|180.00",
        "u8|吴十|13800000008|123456|成都市锦江区春熙路8号|110.00",
        "u9|郑十一|13800000009|123456|武汉市洪山区珞喻路9号|160.00",
        "u10|冯十二|13800000010|123456|西安市雁塔区长安南路10号|95.00",
    ]

    admins = [
        "admin|物流管理员|admin|800.00",
    ]

    couriers = [
        "c1|快递员甲|13900000001|123456|20.00",
        "c2|快递员乙|13900000002|123456|35.00",
        "c3|快递员丙|13900000003|123456|12.50",
        "c4|快递员丁|13900000004|123456|0.00",
        "c5|快递员戊|13900000005|123456|8.00",
        "c6|快递员己|13900000006|123456|18.00",
    ]

    # Parcel fields:
    # id|sender|receiver|description|sentAt|receivedAt|fee|status|itemType|amount|courier|pickedAt
    # status: 0 waiting sign, 1 signed, 2 waiting pickup
    # itemType: 0 standard, 1 fragile, 2 book
    parcels = [
        parcel(1, "u1", "u2", "未分配普通包裹-衣物", "1781600000", "", "15.00", 2, 0, 3, "", ""),
        parcel(2, "u3", "u4", "未分配易碎品-相机", "1781600300", "", "16.00", 2, 1, 2, "", ""),
        parcel(3, "u5", "u1", "未分配图书-教材", "1781600600", "", "6.00", 2, 2, 3, "", ""),
        parcel(4, "u7", "u8", "未分配普通包裹-鞋盒", "1781600900", "", "10.00", 2, 0, 2, "", ""),
        parcel(5, "u9", "u10", "未分配易碎品-玻璃杯", "1781601200", "", "8.00", 2, 1, 1, "", ""),
        parcel(6, "u2", "u6", "c1待揽收-日用品", "1781601500", "", "10.00", 2, 0, 2, "c1", ""),
        parcel(7, "u4", "u3", "c1待揽收-办公用品", "1781601800", "", "5.00", 2, 0, 1, "c1", ""),
        parcel(8, "u8", "u5", "c2待揽收-儿童图书", "1781602100", "", "8.00", 2, 2, 4, "c2", ""),
        parcel(9, "u10", "u7", "c2待揽收-厨房用品", "1781602400", "", "15.00", 2, 0, 3, "c2", ""),
        parcel(10, "u6", "u9", "c3待揽收-易碎茶具", "1781602700", "", "24.00", 2, 1, 3, "c3", ""),
        parcel(11, "u1", "u4", "c4待揽收-运动装备", "1781603000", "", "20.00", 2, 0, 4, "c4", ""),
        parcel(12, "u5", "u2", "c5待揽收-小说套装", "1781603300", "", "12.00", 2, 2, 6, "c5", ""),
        parcel(13, "u4", "u3", "待签收-易碎水杯", "1781603600", "", "8.00", 0, 1, 1, "c3", "1781607200"),
        parcel(14, "u7", "u8", "待签收-图书资料", "1781603900", "", "10.00", 0, 2, 5, "c1", "1781607500"),
        parcel(15, "u2", "u1", "待签收-普通文件箱", "1781604200", "", "5.00", 0, 0, 1, "c2", "1781607800"),
        parcel(16, "u10", "u6", "待签收-易碎摆件", "1781604500", "", "16.00", 0, 1, 2, "c6", "1781608100"),
        parcel(17, "u3", "u9", "待签收-参考书", "1781604800", "", "14.00", 0, 2, 7, "c5", "1781608400"),
        parcel(18, "u6", "u5", "已签收-普通小包", "1781590000", "1781597200", "5.00", 1, 0, 1, "c2", "1781593600"),
        parcel(19, "u8", "u7", "已签收-易碎花瓶", "1781580000", "1781587200", "24.00", 1, 1, 3, "c3", "1781583600"),
        parcel(20, "u1", "u10", "已签收-技术书籍", "1781570000", "1781577200", "18.00", 1, 2, 9, "c1", "1781573600"),
        parcel(21, "u9", "u2", "已签收-家居用品", "1781560000", "1781567200", "25.00", 1, 0, 5, "c4", "1781563600"),
        parcel(22, "u4", "u6", "已签收-易碎餐具", "1781550000", "1781557200", "32.00", 1, 1, 4, "c6", "1781553600"),
        parcel(23, "u5", "u8", "已签收-儿童绘本", "1781540000", "1781547200", "16.00", 1, 2, 8, "c5", "1781543600"),
        parcel(24, "u3", "u1", "已签收-办公文件", "1781530000", "1781537200", "10.00", 1, 0, 2, "c2", "1781533600"),
    ]

    write_lines(data_dir / "users.txt", users)
    write_lines(data_dir / "admins.txt", admins)
    write_lines(data_dir / "couriers.txt", couriers)
    write_lines(data_dir / "parcels.txt", parcels)

    print(f"Seeded Stage 3 demo data in {data_dir}")
    print("Reset after manual testing: rerun this command.")


if __name__ == "__main__":
    main()
