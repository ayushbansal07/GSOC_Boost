#include <boost/gil.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/assert.hpp>
#include <iostream>

using namespace boost::gil;

enum SE_result{
	FIT,
	HIT,
	NONE
};

template <typename SrcView, typename SE>
SE_result find_result(SrcView const &src_view, SE const &se, std::size_t src_row, std::size_t src_col)
{
	std::size_t se_height = se.height();
    std::size_t se_width = se.width();
    int num_hits = 0;
    int num_ones = 0;
    for(std::size_t se_row = 0; se_row < se_height; ++se_row)
    {
    	for(std::size_t se_col = 0; se_col < se_width; ++se_col)
    	{
    		if(se(se_col,se_row) == '1')
    		{
    			++num_ones;
    			if(src_view(src_col + se_col - se_width/2,src_row + se_row - se_height/2)=='1') ++num_hits;
    		}
    	}
    }
    if (num_hits == num_ones)
    	return FIT;
    else if(num_hits > 0)
    	return HIT;
    else
    	return NONE;
}

template <typename SrcView, typename DstView, typename SE>
void morph_impl(SrcView const &src_view, DstView &dst_view, SE const &se, const std::function<bool(SE_result)>& comp_func)
{
	BOOST_ASSERT(src_view.dimensions() == dst_view.dimensions());
    BOOST_ASSERT(se.size() != 0);

    std::size_t height = src_view.height();
    std::size_t width = dst_view.width();
    std::size_t offset_height = se.height()/2;
    std::size_t offset_width = se.width()/2;

    for(std::size_t src_row = offset_height; src_row < height - offset_width; ++src_row)
    {
    	for(std::size_t src_col = offset_width; src_col < width - offset_width; ++src_col)
    	{
    		SE_result result = find_result<SrcView, SE>(src_view, se, src_row, src_col);
    		if(comp_func(result)) dst_view(src_col,src_row) = 1;
    		else dst_view(src_col,src_row) = 0;
    	}
    }
}

template <typename SrcView, typename DstView, typename SE>
void dilate(SrcView const &src_view, DstView &dst_view, SE const &se)
{
	auto lambda = [](SE_result result){return result == FIT || result == HIT;};
	morph_impl(src_view, dst_view, se, lambda);
}

template <typename SrcView, typename DstView, typename SE>
void erode(SrcView const &src_view, DstView &dst_view, SE const &se)
{
	auto lambda = [](SE_result result){return result == FIT;};
	morph_impl(src_view, dst_view, se, lambda);
}




int main()
{
	using namespace std;
	char se[3][3] = {{'0','1','0'},{'1','1','1'},{'0','1','0'}};
	char img_d[5][5];
	char img_e[5][5];
	for(int i=0;i<5;i++)
	{
		for(int j=0;j<5;j++)
		{
			cin>>img_d[i][j];
		}
	}
	for(int i=0;i<5;i++)
	{
		for(int j=0;j<5;j++)
		{
			cin>>img_e[i][j];
		}
	}
	char op_d[5][5] = {0};
	char op_e[5][5] = {0};


	gray8c_view_t img_g_d = interleaved_view(5, 5, (const gray8_pixel_t*)img_d,sizeof(img_d[0]));
	gray8c_view_t img_g_e = interleaved_view(5, 5, (const gray8_pixel_t*)img_e,sizeof(img_e[0]));
	gray8s_view_t op_g_d = interleaved_view(5, 5, (gray8s_pixel_t*)op_d,sizeof(op_d[0]));
	gray8s_view_t op_g_e = interleaved_view(5, 5, (gray8s_pixel_t*)op_e,sizeof(op_e[0]));
	gray8c_view_t se_g = interleaved_view(3, 3, (const gray8_pixel_t*)se,sizeof(se[0]));

	cout<<"SE:START"<<endl;
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			cout<<se_g(i,j)<<" ";
		}
		cout<<endl;
	}
	cout<<"SE:END"<<endl;

	cout<<"IMG_D:START"<<endl;
	for(int i=0;i<5;i++)
	{
		for(int j=0;j<5;j++)
		{
			cout<<img_g_d(i,j)<<" ";
		}
		cout<<endl;
	}
	cout<<"IMG_D:END"<<endl;

	cout<<"IMG_E:START"<<endl;
	for(int i=0;i<5;i++)
	{
		for(int j=0;j<5;j++)
		{
			cout<<img_g_e(i,j)<<" ";
		}
		cout<<endl;
	}
	cout<<"IMG_E:END"<<endl;

	dilate<gray8c_view_t, gray8s_view_t, gray8c_view_t>(img_g_d, op_g_d, se_g);
	erode<gray8c_view_t, gray8s_view_t, gray8c_view_t>(img_g_e, op_g_e, se_g);


	cout<<"OP_D:START"<<endl;
	for(int i=0;i<5;i++)
	{
		for(int j=0;j<5;j++)
		{
			cout<<int(op_g_d(i,j))<<" ";
		}
		cout<<endl;
	}
	cout<<"OP_D:END"<<endl;

	cout<<"OP_E:START"<<endl;
	for(int i=0;i<5;i++)
	{
		for(int j=0;j<5;j++)
		{
			cout<<int(op_g_e(i,j))<<" ";
		}
		cout<<endl;
	}
	cout<<"OP_E:END"<<endl;
}