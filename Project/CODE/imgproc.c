#include "imgproc.h"
#include "common.h"

#define AT AT_IMAGE

int clip(int x, int low, int up){
    return x>up?up:x<low?low:x;
}

AT_ITCM_SECTION_INIT(void clone_image(image_t* img0, image_t* img1)){
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(img0 != img1 && img0->data != img1->data);
    
    if(img0->width == img0->step && img1->width == img1->step){
        memcpy(img1->data, img0->data, img0->width*img0->height);
    }else{
        for(int y=0; y<img0->height; y++){
            memcpy(&AT(img1, 0, y), &AT(img0, 0, y), img0->width);
        }
    }
}

AT_ITCM_SECTION_INIT(void clear_image(image_t* img)){
    assert(img && img->data);
    if(img->width == img->step){
        memset(img->data, 0, img->width*img->height);
    }else{
        for(int y=0; y<img->height; y++){
            memset(&AT(img, 0, y), 0, img->width);
        }
    }
}

AT_ITCM_SECTION_INIT(void threshold(image_t* img0, image_t* img1, uint8_t thres)){
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    
    // 先遍历y后遍历x比较cache-friendly
    for(int y=0; y<img0->height; y++){
        for(int x=0; x<img0->width; x++){
            AT(img1, x, y) = AT(img0, x, y) < thres ? 0 : 255;
        }
    }
}

AT_ITCM_SECTION_INIT(void threshold_inv(image_t* img0, image_t* img1, uint8_t thres)){
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    
    // 先遍历y后遍历x比较cache-friendly
    for(int y=0; y<img0->height; y++){
        for(int x=0; x<img0->width; x++){
            AT(img1, x, y) = AT(img0, x, y) < thres ? 255 : 0;
        }
    }
}

AT_ITCM_SECTION_INIT(void image_and(image_t* img0, image_t* img1, image_t *img2)){
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img2 && img2->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(img0->width == img2->width && img0->height == img2->height);
    
    // 先遍历y后遍历x比较cache-friendly
    for(int y=0; y<img0->height; y++){
        for(int x=0; x<img0->width; x++){
            AT(img2, x, y) = (AT(img0, x, y) == 0 || AT(img1, x, y) == 0) ? 0 : 255;
        }
    }
}

AT_ITCM_SECTION_INIT(void image_or(image_t* img0, image_t* img1, image_t *img2)){
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img2 && img2->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(img0->width == img2->width && img0->height == img2->height);
    
    // 先遍历y后遍历x比较cache-friendly
    for(int y=0; y<img0->height; y++){
        for(int x=0; x<img0->width; x++){
            AT(img2, x, y) = (AT(img0, x, y) == 0 && AT(img1, x, y) == 0) ? 0 : 255;
        }
    }
}


AT_ITCM_SECTION_INIT(void minpool2(image_t *img0, image_t* img1)){
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->width/2 == img1->width && img0->height/2 == img1->height);
    assert(img0 != img1 && img0->data != img1->data);
    
    uint8_t min_value;
    // 先遍历y后遍历x比较cache-friendly
    for(int y=1; y<img0->height; y+=2){
        for(int x=1; x<img0->width; x+=2){
            min_value = 255;
            if(AT(img0, x, y) < min_value) min_value = AT(img0, x, y);
            if(AT(img0, x-1, y) < min_value) min_value = AT(img0, x-1, y);
            if(AT(img0, x, y-1) < min_value) min_value = AT(img0, x, y-1);
            if(AT(img0, x-1, y-1) < min_value) min_value = AT(img0, x-1, y-1);
            AT(img1, x/2, y/2) = min_value;
        }
    }
}

AT_ITCM_SECTION_INIT(void blur(image_t* img0, image_t* img1, uint32_t kernel)){
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(img0 != img1 && img0->data != img1->data);
    
    // 先遍历y后遍历x比较cache-friendly
    for(int y=1; y<img0->height-1; y++){
        for(int x=1; x<img0->width-1; x++){
            AT(img1, x, y) = (1*AT(img0, x-1, y-1) + 2*AT(img0, x, y-1) + 1*AT(img0, x+1, y-1) + 
                              2*AT(img0, x-1,   y) + 4*AT(img0, x,   y) + 2*AT(img0, x+1,   y) + 
                              1*AT(img0, x-1, y+1) + 2*AT(img0, x, y+1) + 1*AT(img0, x+1, y+1)) / 16;
        }
    }
}

AT_ITCM_SECTION_INIT(void sobel3(image_t* img0, image_t* img1)){
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(img0 != img1 && img0->data != img1->data);
    
    int gx, gy;
    // 先遍历y后遍历x比较cache-friendly    
    for(int y=1; y<img0->height-1; y++){
        for(int x=1; x<img0->width-1; x++){
            gx = (-1*AT(img0, x-1, y-1) + 1*AT(img0, x+1, y-1) + 
                  -2*AT(img0, x-1,   y) + 2*AT(img0, x+1,   y) + 
                  -1*AT(img0, x-1, y+1) + 1*AT(img0, x+1, y+1)) / 4;
            gy = ( 1*AT(img0, x-1, y-1) + 2*AT(img0, x, y-1) + 1*AT(img0, x+1, y-1) + 
                  -1*AT(img0, x-1, y+1) - 2*AT(img0, x, y+1) - 1*AT(img0, x+1, y+1)) / 4;
            AT(img1, x, y) = (abs(gx) + abs(gy)) / 2;
        }
    }
}

AT_ITCM_SECTION_INIT(void erode3(image_t* img0, image_t* img1)){
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(img0 != img1 && img0->data != img1->data);
    
    int min_value;
    // 先遍历y后遍历x比较cache-friendly    
    for(int y=1; y<img0->height-1; y++){
        for(int x=1; x<img0->width-1; x++){
            min_value = 255;
            for(int dy=-1; dy<=1; dy++){
                for(int dx=-1; dx<=1; dx++){
                    if(AT(img0, x+dx, y+dy) < min_value) min_value = AT(img0, x+dx, y+dy);
                }
            }
            AT(img1, x, y) = min_value;
        }
    }
}

AT_ITCM_SECTION_INIT(void dilate3(image_t* img0, image_t* img1)){
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(img0 != img1 && img0->data != img1->data);
    
    int max_value;
    // 先遍历y后遍历x比较cache-friendly    
    for(int y=1; y<img0->height-1; y++){
        for(int x=1; x<img0->width-1; x++){
            max_value = 0;
            for(int dy=-1; dy<=1; dy++){
                for(int dx=-1; dx<=1; dx++){
                    if(AT(img0, x+dx, y+dy) > max_value) max_value = AT(img0, x+dx, y+dy);
                }
            }
            AT(img1, x, y) = max_value;
        }
    }
}

AT_ITCM_SECTION_INIT(void remap(image_t* img0, image_t* img1, fimage_t* mapx, fimage_t* mapy)){
    assert(img0 && img0->data);
    assert(img1 && img1->data);
    assert(mapx && mapx->data);
    assert(mapy && mapy->data);
    assert(img0 != img1 && img0->data != img1->data);
    assert(img0->width == img1->width && img0->height == img1->height);
    assert(mapx->width == mapy->width && mapx->height == mapy->height);
    assert(img0->width == mapx->width && img0->height == mapx->height);
    
    // 先遍历y后遍历x比较cache-friendly    
    for(int y=1; y<img0->height-1; y++){
        for(int x=1; x<img0->width-1; x++){
            AT(img1, x, y) = AT(img0, (int)(AT(mapx, x, y)+0.5), (int)(AT(mapy, x, y)+0.5));
        }
    }
}

/* 前进方向定义：
 *   0
 * 3   1
 *   2
 */
AT_DTCM_SECTION_ALIGN_INIT(const int dir_front[4][2], 8) = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
AT_DTCM_SECTION_ALIGN_INIT(const int dir_frontleft[4][2], 8) = {{-1, -1}, {1, -1}, {1, 1}, {-1, 1}};
AT_DTCM_SECTION_ALIGN_INIT(const int dir_frontright[4][2], 8) = {{1, -1}, {1, 1}, {-1, 1}, {-1, -1}};

AT_ITCM_SECTION_INIT(void findline_lefthand_with_thres(image_t* img, uint8_t thres, int x, int y, int pts[][2], int *num)){
    assert(img && img->data);
    assert(num && *num >= 0);
    
    int step=0, dir=0;
    while(step<*num && 0<x && x<img->width-1 && 0<y && y<img->height-1){
        if(AT(img, x+dir_front[dir][0], y+dir_front[dir][1]) < thres){
            dir = (dir+1)%4;
        }else if(AT(img, x+dir_frontleft[dir][0], y+dir_frontleft[dir][1]) < thres){
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
        }else{
            x += dir_frontleft[dir][0];
            y += dir_frontleft[dir][1];
            dir = (dir+3)%4;
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
        }
    }
    *num = step;
}

AT_ITCM_SECTION_INIT(void findline_righthand_with_thres(image_t* img, uint8_t thres, int x, int y, int pts[][2], int *num)){
    assert(img && img->data);
    assert(num && *num >= 0);
    
    int step=0, dir=0;
    while(step<*num && 0<x && x<img->width-1 && 0<y && y<img->height-1){
        if(AT(img, x+dir_front[dir][0], y+dir_front[dir][1]) < thres){
            dir = (dir+3)%4;
        }else if(AT(img, x+dir_frontright[dir][0], y+dir_frontright[dir][1]) < thres){
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
        }else{
            x += dir_frontright[dir][0];
            y += dir_frontright[dir][1];
            dir = (dir+1)%4;
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
        }
    }
    *num = step;
}


AT_ITCM_SECTION_INIT(void approx_lines(int pts[][2], int pts_num, float epsilon, int lines[][2], int* lines_num)){
    assert(pts);
    assert(epsilon > 0);
    
    int dx = pts[pts_num-1][0] - pts[0][0];
    int dy = pts[pts_num-1][1] - pts[0][1];
    float nx = -dy / sqrtf(dx*dx+dy*dy);
    float ny = dx / sqrtf(dx*dx+dy*dy);
    float max_dist = 0, dist;
    int idx = -1;
    for(int i=1; i<pts_num-1; i++){
        dist = fabs((pts[i][0]-pts[0][0])*nx + (pts[i][1]-pts[0][1])*ny);
        if(dist > max_dist){
            max_dist = dist;
            idx = i;
        }
    }
    if(max_dist >= epsilon){
        int num1 = *lines_num;
        approx_lines(pts, idx+1, epsilon, lines, &num1);
        int num2 = *lines_num - num1 - 1;
        approx_lines(pts+idx, pts_num-idx, epsilon, lines+num1-1, &num2);
        *lines_num = num1 + num2 - 1;
    }else{
        lines[0][0] = pts[0][0];
        lines[0][1] = pts[0][1];
        lines[1][0] = pts[pts_num-1][0];
        lines[1][1] = pts[pts_num-1][1];
        *lines_num = 2;
    }
}

AT_ITCM_SECTION_INIT(void approx_lines_f(float pts[][2], int pts_num, float epsilon, float lines[][2], int* lines_num)){
    assert(pts);
    assert(epsilon > 0);
    
    int dx = pts[pts_num-1][0] - pts[0][0];
    int dy = pts[pts_num-1][1] - pts[0][1];
    float nx = -dy / sqrtf(dx*dx+dy*dy);
    float ny = dx / sqrtf(dx*dx+dy*dy);
    float max_dist = 0, dist;
    int idx = -1;
    for(int i=1; i<pts_num-1; i++){
        dist = fabs((pts[i][0]-pts[0][0])*nx + (pts[i][1]-pts[0][1])*ny);
        if(dist > max_dist){
            max_dist = dist;
            idx = i;
        }
    }
    if(max_dist >= epsilon){
        int num1 = *lines_num;
        approx_lines_f(pts, idx+1, epsilon, lines, &num1);
        int num2 = *lines_num - num1 - 1;
        approx_lines_f(pts+idx, pts_num-idx, epsilon, lines+num1-1, &num2);
        *lines_num = num1 + num2 - 1;
    }else{
        lines[0][0] = pts[0][0];
        lines[0][1] = pts[0][1];
        lines[1][0] = pts[pts_num-1][0];
        lines[1][1] = pts[pts_num-1][1];
        *lines_num = 2;
    }
}

void draw_line(image_t* img, int pt0[2], int pt1[2], uint8_t value){
    int dx = pt1[0] - pt0[0];
    int dy = pt1[1] - pt0[1];
    if(abs(dx) > abs(dy)){
        for(int x=pt0[0]; x!=pt1[0]; x+=(dx>0?1:-1)){
            int y=pt0[1]+(x-pt0[0])*dy/dx;
            AT(img, clip(x, 0, img->width-1), clip(y, 0, img->height-1)) = value;
        }
    }else{
        for(int y=pt0[1]; y!=pt1[1]; y+=(dy>0?1:-1)){
            int x=pt0[0]+(y-pt0[1])*dx/dy;
            AT(img, clip(x, 0, img->width-1), clip(y, 0, img->height-1)) = value;
        }
    }
}
