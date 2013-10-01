#include "www.h"

int bbsleft_main()
{
    char /*buf[1024],*/ *ptr;

    init_all();
    check_mybrd();
    printf("<style type=text/css>\n");
    printf("A {color: #000080}\n");
    printf("BODY {BACKGROUND-COLOR: #c0c0c0; }\n");
    //printf("BODY {BACKGROUND-COLOR: #c0c0c0; FONT-SIZE: 12px;}\n");
    printf("</style>\n");
    printf("
           <script>
           function closebut(x, y) {
           if(document.img0) document.img0.src='"UPLOAD_SERVER_HEADER"/folder.gif';
           if(document.img1) document.img1.src='"UPLOAD_SERVER_HEADER"/folder.gif';
           if(document.img2) document.img2.src='"UPLOAD_SERVER_HEADER"/folder.gif';
           if(document.img3) document.img3.src='"UPLOAD_SERVER_HEADER"/folder.gif';
           if(document.img4) document.img4.src='"UPLOAD_SERVER_HEADER"/folder.gif';
           if(document.img5) document.img5.src='"UPLOAD_SERVER_HEADER"/folder.gif';
           if(document.getElementById('div0')) document.getElementById('div0').style.display='none';
           if(document.getElementById('div1')) document.getElementById('div1').style.display='none';
           if(document.getElementById('div2')) document.getElementById('div2').style.display='none';
           if(document.getElementById('div3')) document.getElementById('div3').style.display='none';
           if(document.getElementById('div4')) document.getElementById('div4').style.display='none';
           if(document.getElementById('div5')) document.getElementById('div5').style.display='none';
           x.style.display='block';
           y.src='"UPLOAD_SERVER_HEADER"/folder2.gif';
       }
           function t(x, y) {
           if(x.style.display!='none') {
           x.style.display='none';
           y.src='"UPLOAD_SERVER_HEADER"/folder.gif';
       }
           else
           closebut(x, y);
       }
           function openchat() {
           url='bbschat';
           chatWidth=screen.availWidth;
           chatHeight=screen.availHeight;
           winPara='toolbar=no,location=no,status=no,menubar=no,scrollbars=auto,resizable=yes,left=0,top=0,'+'width='+(chatWidth-10)+',height='+(chatHeight-27);
           window.open(url,'_blank',winPara);
       }
           </script>
           ");
    printf("<nobr>\n");
    if (!loginok )
    {
        printf("<center>
               <form action=bbslogin target=_top method=post><br>
               用户登录<br>
               帐号 <input style='height:20px;' type=text name=id maxlength=12 size=8><br>
               密码 <input style='height:20px;' type=password name=pw maxlength=12 size=8><br>
               <input style='width:72px; height:22px; BACKGROUND-COLOR: #d0f0c0' type=submit value=登录进站>
	       </form>
               </center>
               <body>
               ");
    }
    else
    {
        char buf[256] = "未注册用户";

        printf("<br>\n用户: <a href=bbsqry?userid=%s target=f3>%s</a><br>",
               currentuser.userid, currentuser.userid);
        //                if(currentuser.userlevel & PERM_LOGINOK) strcpy(buf, cexp(countexp(&currentuser), currentuser.user_exp_type));
        if (currentuser.userlevel & PERM_LOGINOK)
            strcpy(buf, "普通站友");
        if (currentuser.userlevel & PERM_BOARDS)
            strcpy(buf, "板主");
        if (currentuser.userlevel & PERM_XEMPT)
            strcpy(buf, "永久帐号");
        if (currentuser.userlevel & PERM_CHATCLOAK)
            strcpy(buf, "本站智囊");
        if (currentuser.userlevel & PERM_SYSOP)
            strcpy(buf, "本站站长");
        printf
        ("级别: %s<hr style='color:2020f0; height:1px' width=84px align=left>",
         buf);
        printf("<a href=bbslogout target=_top>注销本次登录</a><br>\n");
    }
        printf("<center><form action=bbssel target=f3>\n");
        printf
        ("选择讨论区: <br><input type=text name=board maxlength=15 size=8>");
        //printf(" <input type=submit value=确定>");
        printf("</form></center>\n");
    printf
    ("<hr style='color:2020f0; height=1px' width=84px align=left>\n");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link0.gif><a target=f3 href=bbssec>讨论区首页</a><br>\n");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link0.gif><a target=f3 href=bbs0an>精华公布栏</a><br>\n");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link0.gif><a target=f3 href=bbstop10>今日十大</a><br>\n");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link0.gif><a target=f3 href=bbstopb10>热门讨论区</a><br>\n");
    if (loginok)
    {
        /*FILE *fp;*/
        int i, mybrdnum = 0;
        char mybrd[GOOD_BRC_NUM][80];

        printf
        ("<img src="UPLOAD_SERVER_HEADER"/folder.gif name=img0><a href=\"javascript: t(document.getElementById('div0'), document.img0)\">我的收藏夹</a><br>\n");
        printf("<div id=div0 style='display:none'>\n");
        mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
        for (i = 0; i < mybrdnum; i++)
            printf
            ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsdoc?board=%s> %s</a><br>\n",
             mybrd[i], mybrd[i]);
        printf
        ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsmybrd> 预定管理</a><br>\n");
        printf("</div>\n");
    }
    printf("
           <img src="UPLOAD_SERVER_HEADER"/folder.gif name=img1><a href=\"javascript: t(document.getElementById('div1'), document.img1)\">分类讨论区</a><br>
           <div id=div1 style='display:none'>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=0> BBS系统 </a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=1> 上海交大</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=2> 学子院校</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=3> 电脑技术</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=4> 学术科学</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=5> 艺术文化</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=6> 体育运动</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=7> 休闲娱乐</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=8> 知性感性</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=9> 社会信息</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=10> 社团群体</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=11> 游戏专区</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsall> 全部讨论区</a><br>
           </div>
           ");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/folder.gif name=img2><a href=\"javascript: t(document.getElementById('div2'), document.img2)\">谈天说地区</a><br>\n");
    printf("<div id=div2 style='display:none'>\n");
    if (loginok)
    {
        printf
        ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a href=bbsfriend target=f3> 在线好友</a><br>\n");
    }
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a href=bbsusr target=f3> 环顾四方</a><br>\n");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a href=bbsqry target=f3> 查询网友</a><br>\n");
    if (currentuser.userlevel & PERM_PAGE)
    {
        printf
        ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a href=bbssendmsg target=f3> 发送讯息</a><br>\n");
        printf
        ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a href=bbsmsg target=f3> 查看所有讯息</a><br>\n");
    }
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a href=showinfo target=f3> 在线情况分析</a><br>\n");
    printf("</div>\n");
    ptr = "";
    if (currentuser.userlevel & PERM_CLOAK)
        ptr =
            "<img src<img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 onclick='return confirm(\"确实切换隐身状态吗?\")' href=bbscloak> 切换隐身</a><br>\n";
    if (loginok)
        printf("
               <img src="UPLOAD_SERVER_HEADER"/folder.gif name=img3><a href=\"javascript: t(document.getElementById('div3'), document.img3)\">个人工具箱</a><br>
               <div id=div3 style='display:none'>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsinfo> 个人资料</a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsplan> 改说明档<a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbssig> 改签名档<a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbspwd> 修改密码</a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsparm> 修改个人参数</a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsmywww> WWW个人定制</a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsnick> 临时改昵称</a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsfall> 设定好友</a><br>
               %s</div>
               <img src="UPLOAD_SERVER_HEADER"/folder.gif name=img5><a href=\"javascript: t(document.getElementById('div5'), document.img5)\">处理信件区</a><br>
               <div id=div5 style='display:none'>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsnewmail> 阅览新邮件</a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsmail> 所有邮件</a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbspstmail> 发送邮件</a><br>
               </div>
               ", ptr);
    printf("<img src="UPLOAD_SERVER_HEADER"/folder.gif name=img4>");
    printf
    ("<a href=\"javascript: t(document.getElementById('div4'), document.img4)\">特别服务区</a><br>\n");
    printf("<div id=div4 style='display:none'>\n");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsnotepad>看看留言板</a><br>\n");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsalluser>所有使用者</a><br>\n");
    //printf("<img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsmovie>活动看板</a><br>\n");
    if (currentuser.userlevel & PERM_OBOARDS)
    {
        printf
        ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsallf>上载情况</a><br>\n");
        printf
        ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsfind2>上载查询</a><br>\n");
    }
    printf("</div>\n");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link0.gif><a href=bbsfind target=f3>文章查询</a><br>\n");
    //      printf("<img src="UPLOAD_SERVER_HEADER"/link0.gif><a href=bbssel target=f3>查找讨论区</a><br>\n");
    printf("<img src="UPLOAD_SERVER_HEADER"/telnet.gif><a href='telnet:%s'>Telnet登录</a><br>\n",
           BBSHOST);
    if (!loginok)
        printf
        ("<img src="UPLOAD_SERVER_HEADER"/link0.gif><a href=\"javascript:void open('bbsreg', '', 'width=620,height=520')\">新用户注册</a><br>\n");
    if (loginok)
    {
        if (HAS_PERM(PERM_LOGINOK) && !HAS_PERM(PERM_POST))
            printf
            ("<script>alert('您被封禁了全站发表文章的权限, 请参看penalty板公告, 期满后在sysop板申请解封. 如有异议, 可在sysop板提出申诉.')</script>\n");
        if (count_new_mails() > 0)
            printf("<script>alert('您有新信件!')</script>\n");
    }
    if (loginok && !(currentuser.userlevel & PERM_LOGINOK)
            && !has_fill_form())
        printf
        ("<br><a style='color:red; font-size:15px' target=f3 href=bbsform><b>填写注册单</b></a><br>\n");
    printf("</body>");
}

int count_new_mails()
{
    struct fileheader x1;
    int unread = 0;
    char buf[1024];
    FILE *fp;

    if (currentuser.userid[0] == 0)
        return 0;
    sprintf(buf, "%s/mail/%c/%s/.DIR", BBSHOME,
            toupper(currentuser.userid[0]), currentuser.userid);
    fp = fopen(buf, "r");
    if (fp == 0)
        return;
    while (fread(&x1, sizeof(x1), 1, fp) > 0)
        if (!(x1.accessed[0] & FILE_READ))
            unread++;
    fclose(fp);
    return unread;
}

int check_mybrd()
{
    FILE *fp;
    char file[200];
    char mybrd[GOOD_BRC_NUM][80], mybrd2[GOOD_BRC_NUM][80];
    int i, total = 0, mybrdnum = 0;

    if (!loginok)
        return;
    mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
    total = 0;
    for (i = 0; i < mybrdnum; i++)
    {
        if (has_read_perm(&currentuser, mybrd[i]))
        {
            strcpy(mybrd2[total], mybrd[i]);
            total++;
        }
    }
    if (total == mybrdnum)
        return;
    fp = fopen(file, "w");
    fwrite(mybrd2, 80, total, fp);
    fclose(fp);
}
