#!/usr/bin/perl -w

#测试说明：这里处理 2 个文件，一个作为参数，另外作为标准输入"<>" 
#END 前面部分处理参数文件‘pp’ 后部分处理 标准输入，记得用大括号包住
#pp k-v 数据库， a 目标替换文件
#cat a|perl -alne '$hash{@F[0]}=@F[1];END{while(<>){/(.*)\n/;print $hash{$+}}}' pp

#final 
cat abc.yaml|perl -ane '$hash{@F[0]}=@F[1];END{while(<>){$_ =~ s/($+)/$hash{$+}/ if /\s+server: (.*)\n/;print $_}}' pp  >xdfsadf.yaml
