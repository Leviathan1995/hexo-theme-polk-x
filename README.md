## Polk-x
极简Hexo博客主题, 基于[polk](https://github.com/chunqiuyiyu/hexo-theme-polk)主题修改.

## Preview
[My Blog](http://www.leviathan.vip)
![preview](https://github.com/Leviathan1995/hexo-theme-polk-x/blob/master/demo.jpg?raw=true)

## Install

```shell
cd your-blog
git clone https://github.com/Leviathan1995/hexo-theme-polk-x.git themes/polk-x
```

### 修改主题
修改博客根目录下`_config.yml`
```
theme: polk-x
```
为了更好的显示效果,请将页面渲染文章数量`per_page`改为20(默认为10).

## 添加RSS订阅
* 安装RSS插件
```shell
npm install hexo-generator-feed
```

* 修改博客根目录下`_config.yml`
```yml
# RSS
rss: true
plugin:
  - hexo-generator-feed

# Feed Atom
feed:
  type: atom
  path: atom.xml
  limit: 20
  target: _blank
```
* 重新部署
```shell
hexo clean
hexo g
```

## 添加评论插件
添加评论插件代码至`layout/post.ejs`的注释`<!-- 评论插件 -->`下方即可.

## 浏览器支持
- Internet Explorer 9+
- Firefox
- Opera
- Chrome
- Safari
- Edge

## Thanks
* [polk](https://github.com/chunqiuyiyu/hexo-theme-polk)

## Contribution
Issues and pull requests are welcome

## License
[MIT](LICENSE)
